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

#pragma once

#include <atomic>
#include <deque>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <SDL_opengl.h>

#include "atlas/tile.h"

namespace Turbine
{

class TileStreamer
{
public:
	TileStreamer();
	~TileStreamer();

	// Gets a tile for a given zoom level. If a tile is "static", it will never be streamed out once loaded.
	TileSharedPtr Get(int x, int y, int zoomLevel, bool isStatic);

	void Update(float delta);

	static void ShowDebugUI(bool* pOpen);
	
private:
	static int TileStreamerThreadMain(TileStreamer* pTileRequester);
	static bool LoadFromFile(Tile& tile);
	static bool DownloadFromTileServer(Tile& tile); 
	void CreateDirectories();

	struct TileStreamInfo 
	{
		TileStreamInfo() 
		{
			pTile = nullptr;
			isStatic = false;
			accessTimer = 0.0f;
		}

		TileSharedPtr pTile;
		bool isStatic;
		float accessTimer;
	};
	using TileStreamInfoDeque = std::deque<TileStreamInfo>;
	using TileStreamInfoList = std::list<TileStreamInfo>;

	std::mutex m_AccessMutex;
	TileStreamInfoDeque m_Queue;
	TileStreamInfoList m_LoadedTiles;
	TileStreamInfo m_LoadingTile;
	std::thread m_Thread;
	std::atomic_bool m_RunThread;
};

} // namespace Watcher
