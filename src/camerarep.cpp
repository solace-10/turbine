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

#include "imgui/imgui.h"
#include "camerarep.h"
#include "turbine.h"

namespace Turbine
{

CameraRep::CameraRep(CameraWeakPtr cameraWeakPtr)
{
	m_CameraWeakPtr = cameraWeakPtr;
	m_WindowWidth = 480.0f;
	m_WindowHeight = 320.0f;
	m_TextureWidth = 0;
	m_TextureHeight = 0;
	glGenTextures(1, &m_Texture);
	m_Open = true;
}

CameraWeakPtr CameraRep::GetCameraWeakPtr() const
{
	return m_CameraWeakPtr;
}

bool CameraRep::IsOpen() const
{
	return m_Open;
}

void CameraRep::Close()
{
	m_Open = false;
	glDeleteTextures(1, &m_Texture);
}

GLuint CameraRep::GetTexture() const
{
	return m_Texture;
}

void CameraRep::ConstrainedRatio(ImGuiSizeCallbackData* pData) 
{ 
	CameraRep* pCameraRep = reinterpret_cast<CameraRep*>(pData->UserData);
	const float ratio = static_cast<float>(pCameraRep->m_TextureHeight) / static_cast<float>(pCameraRep->m_TextureWidth);
	pData->DesiredSize.y = pData->DesiredSize.x * ratio;
	pCameraRep->m_WindowWidth = pData->DesiredSize.x;
	pCameraRep->m_WindowHeight = pData->DesiredSize.y;
}

void CameraRep::Update()
{

}

void CameraRep::Render()
{
	if (!m_Open)
	{
		return;
	}

	CameraSharedPtr pCamera = m_CameraWeakPtr.lock();
	if (pCamera == nullptr)
	{
		return;
	}

	if (m_TextureWidth == 0 && m_TextureHeight == 0)
	{
		ImGui::SetNextWindowSize(ImVec2(m_WindowWidth, m_WindowHeight));
	}
	else
	{
		ImGui::SetNextWindowSizeConstraints(
			ImVec2(0, 0), 
			ImVec2((float)m_TextureWidth, (float)m_TextureHeight), 
			ConstrainedRatio,
			this
		);
	}

	if (ImGui::Begin(pCamera->GetURL().c_str(), &m_Open, ImGuiWindowFlags_NoSavedSettings))
	{
		ImVec2 windowPos = ImGui::GetWindowPos();

		if (m_TextureWidth == 0 && m_TextureHeight == 0)
		{
			glBindTexture(GL_TEXTURE_2D, m_Texture);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_TextureWidth);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_TextureHeight);
			if (m_TextureWidth > 0 && m_TextureHeight > 0)
			{
				m_WindowWidth = static_cast<float>(m_TextureWidth);
				m_WindowHeight = static_cast<float>(m_TextureHeight);

				json message =
				{
					{ "type", "stream_started" },
					{ "url", pCamera->GetURL() }
				};
				g_pTurbine->OnMessageReceived(message);
			}
		}

		ImDrawList* pDrawList = ImGui::GetWindowDrawList();
		ImTextureID cameraTexture = reinterpret_cast<ImTextureID>(GetTexture());
		pDrawList->AddImage(cameraTexture, windowPos, ImVec2(windowPos.x + m_WindowWidth, windowPos.y + m_WindowHeight));

		GeolocationData* pGeo = pCamera->GetGeolocationData();
		ImGui::Text("%s, %s, %s", pGeo->GetCity().c_str(), pGeo->GetRegion().c_str(), pGeo->GetCountry().c_str());
		ImGui::Text("%s", pGeo->GetIPAddress().ToString().c_str());

		if (m_Open == false)
		{
			Close();
		}
	}

	ImGui::End();
}

} // namespace Turbine
