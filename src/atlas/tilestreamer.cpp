/*
MIT License

Copyright (c) 2022 Pedro Nunes

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <sstream>

#include <curl/curl.h>

#include "atlas/atlas.h"
#include "atlas/tilestreamer.h"
#include "atlas/tile.h"
#include "imgui/imgui.h"
#include "log.h"
#include "settings.h"
#include "textureloader.h"
#include "turbine.h"

namespace Turbine
{

TileStreamer* g_pTileStreamer = nullptr;

TileStreamer::TileStreamer()
{
	m_RunThread = true;
	m_Thread = std::thread(&TileStreamer::TileStreamerThreadMain, this);
	CreateDirectories();
	g_pTileStreamer = this;
}

TileStreamer::~TileStreamer()
{
	if (m_Thread.joinable())
	{
		m_RunThread = false;
		m_Thread.join();
	}
	g_pTileStreamer = nullptr;
}

TileSharedPtr TileStreamer::Get(int x, int y, int zoomLevel, bool isStatic)
{
	SDL_assert(x >= 0);
	SDL_assert(y >= 0);
	SDL_assert(zoomLevel >= 0);

	auto containsFn = [x, y, zoomLevel](const TileStreamInfo& tileStreamInfo) -> bool
	{
		return tileStreamInfo.pTile->X() == x && tileStreamInfo.pTile->Y() == y && tileStreamInfo.pTile->ZoomLevel() == zoomLevel;
	};

	std::lock_guard< std::mutex > lock(m_AccessMutex);
	auto loadedTileIt = std::find_if(m_LoadedTiles.begin(), m_LoadedTiles.end(), containsFn);
	if (loadedTileIt != m_LoadedTiles.end())
	{
		loadedTileIt->accessTimer = 0.0f;
		return loadedTileIt->pTile;
	}

	auto queuedTileIt = std::find_if(m_Queue.begin(), m_Queue.end(), containsFn);
	if (queuedTileIt != m_Queue.end())
	{
		return queuedTileIt->pTile;
	}
	else if (m_LoadingTile.pTile && containsFn(m_LoadingTile))
	{
		return m_LoadingTile.pTile;
	}

	TileStreamInfo streamInfo;
	streamInfo.pTile = std::make_shared<Tile>(x, y, zoomLevel);
	streamInfo.isStatic = isStatic;
	m_Queue.push_back(streamInfo);
	return streamInfo.pTile;
}

void TileStreamer::Update(float delta)
{
	std::lock_guard<std::mutex> lock(m_AccessMutex);
	TileStreamInfoList::iterator it = m_LoadedTiles.begin();
	while (it != m_LoadedTiles.end())
	{
		auto& rsi = *it;

		if (rsi.isStatic == false)
		{
			rsi.accessTimer += delta;
		}

		if (rsi.accessTimer > 30.0f)
		{
			it = m_LoadedTiles.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void TileStreamer::ShowDebugUI(bool* pOpen)
{
	if (g_pTileStreamer == nullptr || pOpen == nullptr)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Atlas tile streamer", pOpen))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	g_pTileStreamer->m_AccessMutex.lock();
	ImGui::Text("Tiles loaded: %zu", g_pTileStreamer->m_LoadedTiles.size());

	static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;
	if (ImGui::BeginTable("table", 5, flags))
	{
		ImGui::TableSetupColumn("Zoom");
		ImGui::TableSetupColumn("X");
		ImGui::TableSetupColumn("Y");
		ImGui::TableSetupColumn("Type");
		ImGui::TableSetupColumn("Hidden for");

		ImGui::TableHeadersRow();

		for (const auto& tsi : g_pTileStreamer->m_LoadedTiles)
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("%d", tsi.pTile->ZoomLevel());
			ImGui::TableNextColumn();
			ImGui::Text("%d", tsi.pTile->X());
			ImGui::TableNextColumn();
			ImGui::Text("%d", tsi.pTile->Y());
			ImGui::TableNextColumn();
			ImGui::Text("%s", tsi.isStatic ? "Static" : "Dynamic");
			ImGui::TableNextColumn();

			if (tsi.isStatic)
			{
				ImGui::Text("-");
			}
			else
			{
				ImGui::Text("%.2f", tsi.accessTimer);
			}
		}
		ImGui::EndTable();
	}
	g_pTileStreamer->m_AccessMutex.unlock();

	ImGui::End();
}

int TileStreamer::TileStreamerThreadMain(TileStreamer* pTS)
{
	while (pTS->m_RunThread)
	{
		pTS->m_AccessMutex.lock();
		if (pTS->m_Queue.empty() == false)
		{
			pTS->m_LoadingTile = pTS->m_Queue.front();
			pTS->m_Queue.pop_front();
		}
		pTS->m_AccessMutex.unlock();

		if (pTS->m_LoadingTile.pTile != nullptr)
		{
			Tile& tile = *pTS->m_LoadingTile.pTile;
			if (LoadFromFile(tile) == false)
			{
				DownloadFromTileServer(tile);
				LoadFromFile(tile);
			}

			pTS->m_AccessMutex.lock();
			pTS->m_LoadedTiles.push_front(pTS->m_LoadingTile);
			pTS->m_AccessMutex.unlock();
			pTS->m_LoadingTile.pTile = nullptr;
		}

		if (pTS->m_Queue.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
		}
	}

	return 0;
}

bool TileStreamer::LoadFromFile(Tile& tile)
{
	std::stringstream filename;
	filename << tile.X() << "_" << tile.Y() << ".png";

	std::filesystem::path path = g_pTurbine->GetSettings()->GetStoragePath() / "atlas" / std::to_string(tile.ZoomLevel()) / filename.str();

	if (std::filesystem::exists(path) == false)
	{
		return false;
	}
	else
	{
		GLuint texture = TextureLoader::LoadTexture(path);
		if (texture > 0)
		{
			tile.AssignTexture(texture);
			return true;
		}
		else
		{
			return false;
		}
	}
}

static size_t WriteTileFileCallback(void* pBuffer, size_t size, size_t nmemb, void *pStream)
{
	return fwrite(pBuffer, size, nmemb, reinterpret_cast<FILE*>(pStream));
}

bool TileStreamer::DownloadFromTileServer(Tile& tile)
{
	CURL* pCurlHandle;
	std::stringstream url;
	url << "http://a.tile.stamen.com/toner/" << tile.ZoomLevel() << "/" << tile.X() << "/" << tile.Y() << ".png";

	std::stringstream filename;
	filename << tile.X() << "_" << tile.Y() << ".png";

	std::filesystem::path path = g_pTurbine->GetSettings()->GetStoragePath() / "atlas" / std::to_string(tile.ZoomLevel()) / filename.str();
	FILE* pTileFile = nullptr;

	pCurlHandle = curl_easy_init();
	
	curl_easy_setopt(pCurlHandle, CURLOPT_URL, url.str().c_str());
	curl_easy_setopt(pCurlHandle, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(pCurlHandle, CURLOPT_WRITEFUNCTION, &WriteTileFileCallback);

#ifdef _WIN32
	_wfopen_s(&pTileFile, path.c_str(), L"wb");
#else
	pTileFile = fopen(path.c_str(), "wb");
#endif
	bool result = false;
	if (pTileFile != nullptr)
	{
		curl_easy_setopt(pCurlHandle, CURLOPT_WRITEDATA, pTileFile);
		CURLcode code = curl_easy_perform(pCurlHandle);
		fclose(pTileFile);
		if (code != CURLE_OK)
		{
			std::filesystem::remove(path);
		}
	}

	curl_easy_cleanup(pCurlHandle);
	return result;
}

void TileStreamer::CreateDirectories()
{
	for (int zoomLevel = 0; zoomLevel < sMaxZoomLevels; ++zoomLevel)
	{
		std::filesystem::path path = g_pTurbine->GetSettings()->GetStoragePath() / "atlas" / std::to_string(zoomLevel);
		std::filesystem::create_directories(path);
	}
}

} // namespace Turbine
