//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2022 TeamViewer Germany GmbH                                     //
//                                                                                //
// Permission is hereby granted, free of charge, to any person obtaining a copy   //
// of this software and associated documentation files (the "Software"), to deal  //
// in the Software without restriction, including without limitation the rights   //
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      //
// copies of the Software, and to permit persons to whom the Software is          //
// furnished to do so, subject to the following conditions:                       //
//                                                                                //
// The above copyright notice and this permission notice shall be included in all //
// copies or substantial portions of the Software.                                //
//                                                                                //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    //
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  //
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  //
// SOFTWARE.                                                                      //
//********************************************************************************//
#include <TVAgentAPI/tvagentapi.h>

#include <cstdio>
#include <cstdlib>
#include <csignal>

namespace
{

bool s_isInterrupted = false;

void signalHandler(int signo)
{
	if (signo == SIGINT)
	{
		s_isInterrupted = true;
	}
}

void connectionStatusChanged(tvagentapi::IAgentConnection::Status status, void* userdata) noexcept
{
	(void)userdata;
	printf("[IAgentConnection] Status: %s\n", tvagentapi::toCString(status));
}

} // namespace

int main()
{
	if (signal(SIGINT, signalHandler) == SIG_ERR)
	{
		fputs("Failed to set up signal handler\n", stderr);
		return EXIT_FAILURE;
	}

	tvagentapi::IAgentAPI* agentAPI = TVGetAgentAPI();
	if (!agentAPI)
	{
		fputs("Failed to create IAgentAPI\n", stderr);
		return EXIT_FAILURE;
	}

	// create file logging
	tvagentapi::ILogging* logging = agentAPI->createFileLogging("example.log");
	if (!logging)
	{
		fputs("Failed to start file logging\n", stderr);
		return EXIT_FAILURE;
	}

	// connect tvagentapi sdk to IoT Agent
	// We pass logging to AgentConnection, but we still manage its lifetime and must pair it with IAgentAPI::destroyLogging()
	// logging object will be used internally, and we may only release it (in this case by calling IAgentAPI::destroyLogging()
	// after call to agentAPI->destroyAgentConnection();
	tvagentapi::IAgentConnection* agentConnection = agentAPI->createAgentConnection(logging);
	if (!agentConnection)
	{
		fputs("Failed to create connection\n", stderr);
		return EXIT_FAILURE;
	}

	const char* baseSdkUrl = std::getenv("TV_BASE_SDK_URL");
	const char* agentApiUrl = std::getenv("TV_AGENT_API_URL");
	if (baseSdkUrl && agentApiUrl)
	{
		const tvagentapi::IAgentConnection::SetConnectionURLsResult result =
			agentConnection->setConnectionURLs(baseSdkUrl, agentApiUrl);
		if (result != tvagentapi::IAgentConnection::SetConnectionURLsResult::Success)
		{
			fprintf(stderr, "Failed to set connection URLs: %s", tvagentapi::toCString(result));
			return EXIT_FAILURE;
		}
	}

	agentConnection->setStatusChangedCallback({connectionStatusChanged, nullptr/*userdata*/});

	// start connection to IoT Agent
	printf("Connecting to IoT Agent...\n");
	agentConnection->start();

	// main application's event loop
	printf("Monitoring connection status changes... Press Ctrl+C to exit\n");
	while (!s_isInterrupted)
	{
		// all the tvagentapi callbacks will be called on the thread calling processEvents()
		// we wait for 100ms for more events and process them
		agentConnection->processEvents(/*waitForMoreEvents = */true, /*timeoutMs = */100);
	}

	// stop connection to IoT Agent
	printf("Stopping connection to IoT Agent...\n");
	agentConnection->stop();

	// we call processEvents() one more time to receive callbacks emitted by stop()
	agentConnection->processEvents();

	printf("Cleaning up...\n");
	agentAPI->destroyAgentConnection(agentConnection);

	// after destroyAgentConnection() we are sure logging is not used, and we can safely destroy it
	agentAPI->destroyLogging(logging);

	printf("Exiting...\n");

	return EXIT_SUCCESS;
}
