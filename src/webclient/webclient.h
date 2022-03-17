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
#include <functional>
#include <list>
#include <string>
#include <thread>
#include <vector>

#include <curl/curl.h>

#include "webclient/webclientrequestresult.h"

namespace Turbine
{

class WebClient
{
public:
	using RequestCallback = std::function<void(const WebClientRequestResult& result)>;
	using Headers = std::vector<std::string>;

	WebClient();
	~WebClient();

	void Update();

	void Delete(const std::string& url, Headers headers, const std::string& postData, RequestCallback pCallback, bool debug = false);
	void Get(const std::string& url, Headers headers, RequestCallback pCallback, bool debug = false);
	void Post(const std::string& url, Headers headers, const std::string& postData, RequestCallback pCallback, bool debug = false);

	void AppendData(uint32_t id, const std::string& data);

private:
	static void CURLThreadMain(WebClient* pWebClient);
	
	enum class RequestType
	{
		Delete,
		Get,
		Post
	};
	void Request(RequestType requestType, const std::string& url, Headers headers, const std::string& postData, RequestCallback pCallback, bool debug);

	CURLM* m_MultiHandler;
	std::thread m_CURLThread;
	std::atomic_bool m_ThreadRunning;

	struct PendingRequest
	{
		PendingRequest()
		{
			id = 0;
			pCallback = nullptr;
			pHandle = nullptr;
			done = false;
			pHeaders = nullptr;
		}

		uint32_t id;
		std::string url;
		RequestCallback pCallback;
		CURL* pHandle;
		std::string data;
		std::string postData;
		bool done;
		curl_slist* pHeaders;
	};
	std::list<PendingRequest> m_PendingRequests;
};

} // namespace Turbine
