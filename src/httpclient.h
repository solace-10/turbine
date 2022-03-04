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

#include <functional>
#include <list>
#include <string>

#include <curl/curl.h>

namespace Turbine
{

class HTTPRequestResult
{
public:
	HTTPRequestResult(int statusCode, const std::string& data);
	int GetStatusCode() const;
	const std::string& GetData() const;

private:
	int m_StatusCode;
	std::string m_Data;
};

using HTTPRequestCallback = std::function<void(const HTTPRequestResult& result)>;

class HTTPClient
{
public:
	HTTPClient();
	~HTTPClient();

	void Update();
	void Request(const std::string& url, HTTPRequestCallback pCallback);

	void AppendData(uint32_t id, const std::string& data);

private:
	CURLM* m_MultiHandler;

	struct PendingRequest
	{
		uint32_t id;
		std::string url;
		HTTPRequestCallback pCallback;
		CURL* pHandle;
		std::string data;
	};
	std::list<PendingRequest> m_PendingRequests;
};

} // namespace Turbine
