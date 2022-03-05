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

#include "webclient/webclient.h"
#include "turbine.h"

namespace Turbine
{

static size_t CURLWriteCallback(void* pBuffer, size_t size, size_t nmemb, void* pUserData)
{
	if (nmemb > 0)
	{
		std::string data(reinterpret_cast<unsigned char*>(pBuffer), reinterpret_cast<unsigned char*>(pBuffer) + nmemb);
		uint32_t id = reinterpret_cast<uint32_t>(pUserData);
		g_pTurbine->GetWebClient()->AppendData(id, data);
	}

	return nmemb;
}

WebClient::WebClient()
{
	m_MultiHandler = curl_multi_init();
	m_ThreadRunning = true;
	m_CURLThread = std::move(std::thread(&CURLThreadMain, this));
}

WebClient::~WebClient()
{
	m_ThreadRunning = false;
	if (m_CURLThread.joinable())
	{
		m_CURLThread.join();
	}

	for (auto& pr : m_PendingRequests)
	{
		curl_multi_remove_handle(m_MultiHandler, pr.pHandle);
		curl_easy_cleanup(pr.pHandle);
	}

	curl_multi_cleanup(m_MultiHandler);
}

void WebClient::Update()
{
	for (std::list<PendingRequest>::iterator it = m_PendingRequests.begin(); it != m_PendingRequests.end();)
	{
		PendingRequest& pr = *it;
		if (pr.done)
		{
			pr.pCallback(WebClientRequestResult(0, pr.data));
			it = m_PendingRequests.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void WebClient::CURLThreadMain(WebClient* pWebClient)
{
	while (pWebClient->m_ThreadRunning)
	{
		int inProgress = 0;
		curl_multi_perform(pWebClient->m_MultiHandler, &inProgress);

		curl_multi_poll(pWebClient->m_MultiHandler, nullptr, 0, 1000, nullptr);

		CURLMsg* pMessage = nullptr;
		int messagesLeft = 0;
		while ((pMessage = curl_multi_info_read(pWebClient->m_MultiHandler, &messagesLeft))) {
			if (pMessage->msg == CURLMSG_DONE) {

				for (auto& pr : pWebClient->m_PendingRequests)
				{
					if (pMessage->easy_handle == pr.pHandle)
					{
						curl_multi_remove_handle(pWebClient->m_MultiHandler, pr.pHandle);
						curl_easy_cleanup(pr.pHandle);
						if (pr.pHeaders != nullptr)
						{
							curl_slist_free_all(pr.pHeaders);
						}
						pr.pHandle = nullptr;
						pr.done = true;
						break;
					}
				}
			}
		}
	}
}

void WebClient::Get(const std::string& url, Headers headers, RequestCallback pCallback)
{
	Request(RequestType::Get, url, headers, "", pCallback);
}

void WebClient::Post(const std::string& url, Headers headers, const std::string& postData, RequestCallback pCallback)
{
	Request(RequestType::Post, url, headers, postData, pCallback);
}

void WebClient::Request(RequestType requestType, const std::string& url, Headers headers, const std::string& postData, RequestCallback pCallback)
{
	static uint32_t sId = 0;
	PendingRequest pr;
	pr.id = sId++;
	pr.url = url;
	pr.pCallback = pCallback;
	pr.pHandle = curl_easy_init();

	curl_easy_setopt(pr.pHandle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(pr.pHandle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_setopt(pr.pHandle, CURLOPT_CAINFO, "cacert.pem");
	curl_easy_setopt(pr.pHandle, CURLOPT_WRITEFUNCTION, &CURLWriteCallback);
	curl_easy_setopt(pr.pHandle, CURLOPT_WRITEDATA, pr.id);

	if (requestType == RequestType::Post)
	{
		curl_easy_setopt(pr.pHandle, CURLOPT_POSTFIELDS, postData.c_str());
		curl_easy_setopt(pr.pHandle, CURLOPT_POSTFIELDSIZE, postData.size());
		curl_easy_setopt(pr.pHandle, CURLOPT_POST, 1L);
	}

	if (headers.empty() == false)
	{
		for (auto& header : headers)
		{
			pr.pHeaders = curl_slist_append(pr.pHeaders, header.c_str());
		}
		curl_easy_setopt(pr.pHandle, CURLOPT_HTTPHEADER, pr.pHeaders);
	}

	curl_multi_add_handle(m_MultiHandler, pr.pHandle);
	m_PendingRequests.push_back(pr);
}

void WebClient::AppendData(uint32_t id, const std::string& data)
{
	for (auto& pr : m_PendingRequests)
	{
		if (pr.id == id)
		{
			pr.data += data;
			break;
		}
	}
}

} // namespace Turbine
