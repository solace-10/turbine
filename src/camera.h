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

#include <memory>
#include <string>
#include <vector>

#include "geolocationdata.h"
#include "ipaddress.h"

namespace Turbine
{

class Camera;
using CameraSharedPtr = std::shared_ptr<Camera>;
using CameraWeakPtr = std::weak_ptr<Camera>;
using CameraVector = std::vector<CameraSharedPtr>;

class Camera
{
public:
	enum class State
	{
		Unknown = 0,
		Unauthorised,
		StreamAvailable,

		Count
	};

	Camera(const std::string& title, const std::string& url, const IPAddress& address, State cameraState = State::Unknown);

	const std::string& GetTitle() const;
	const std::string& GetURL() const;
	const IPAddress& GetAddress() const;
	GeolocationData* GetGeolocationData() const;
	void SetGeolocationData(GeolocationDataSharedPtr pGeolocationData);
	State GetState() const;
	void SetState(State state);

private:
	std::string m_Title;
	std::string m_URL;
	IPAddress m_Address;
	GeolocationDataSharedPtr m_pGeolocationData;
	State m_State;
};

inline Camera::Camera(const std::string& title, const std::string& url, const IPAddress& address, State cameraState /* = State::Unknown */)
{
	m_Title = title;
	m_URL = url;
	m_Address = address;
	m_State = cameraState;
}

inline const std::string& Camera::GetTitle() const
{
	return m_Title;
}

inline const std::string& Camera::GetURL() const
{
	return m_URL;
}

inline const IPAddress& Camera::GetAddress() const
{
	return m_Address;
}

inline GeolocationData* Camera::GetGeolocationData() const
{
	return m_pGeolocationData.get();
}

inline void Camera::SetGeolocationData(GeolocationDataSharedPtr pGeolocationData)
{
	m_pGeolocationData = pGeolocationData;
}

inline Camera::State Camera::GetState() const
{
	return m_State;
}

inline void Camera::SetState(State state)
{
	m_State = state;
}

} // namespace Turbine
