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
#include <vector>

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

void printRunningSessions(void* userdata)
{
	auto tvSessionManagementModule = static_cast<tvagentapi::ITVSessionManagementModule*>(userdata);

	std::vector<tvagentapi::ITVSessionManagementModule::TVSessionID> tvSessionIDs;

	tvSessionManagementModule->enumerateRunningSessions(tvagentapi::ITVSessionManagementModule::SessionEnumerator{
		[](tvagentapi::ITVSessionManagementModule::TVSessionID tvSessionID, void* userdata) -> bool
		{
			auto outVec = static_cast<std::vector<tvagentapi::ITVSessionManagementModule::TVSessionID>*>(userdata);
			outVec->push_back(tvSessionID);
			return true;
		},
		&tvSessionIDs});

	printf("  Currently %zu running session(s)", tvSessionIDs.size());

	if (tvSessionIDs.empty())
	{
		printf(".\n");
	}
	else
	{
		printf(": { ");
		for (size_t i = 0; i < tvSessionIDs.size(); ++i)
		{
			printf("%d%s", tvSessionIDs[i], (i == tvSessionIDs.size() - 1 ? "" : ", "));
		}
		printf(" }\n");
	}
}

void connectionStatusChanged(tvagentapi::IAgentConnection::Status status, void* userdata)
{
	printf("[IAgentConnection] Status: %s\n", tvagentapi::toCString(status));
	if (status == tvagentapi::IAgentConnection::Status::Connected)
	{
		printRunningSessions(userdata);
	}
}

void sessionStartedCallback(tvagentapi::ITVSessionManagementModule::TVSessionID tvSessionID, int32_t tvSessionCount, void* userdata)
{
	printf("[TVSessionManagementModule] Session Started: id %d (session count %d)\n",
		tvSessionID, tvSessionCount);
	printRunningSessions(userdata);
}

void sessionStoppedCallback(tvagentapi::ITVSessionManagementModule::TVSessionID tvSessionID, int32_t tvSessionCount, void* userdata)
{
	printf("[TVSessionManagementModule] Session Stopped: id %d (session count %d)\n",
		tvSessionID, tvSessionCount);
	printRunningSessions(userdata);
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
	// We pass logging to AgentConnection, but we still manage it's lifetime and must pair it with IAgentAPI::destroyLogging()
	// logging object will be used internally and we may only release it (in this case by calling IAgentAPI::destroyLogging()
	// after call to agentAPI->destroyAgentConnection();
	tvagentapi::IAgentConnection* agentConnection = agentAPI->createAgentConnectionLocal(logging);
	if (!agentConnection)
	{
		fputs("Failed to create connection\n", stderr);
		return EXIT_FAILURE;
	}

	tvagentapi::ITVSessionManagementModule* tvSessionManagementModule =
		tvagentapi::getModule<tvagentapi::ITVSessionManagementModule>(agentConnection);
	if (!tvSessionManagementModule)
	{
		fputs("Failed to create TVSessionManagementModule\n", stderr);
		return EXIT_FAILURE;
	}
	if (!tvSessionManagementModule->isSupported())
	{
		fputs("TVSessionManagementModule not supported\n", stderr);
		return EXIT_FAILURE;
	}

	tvSessionManagementModule->setCallbacks({
			{sessionStartedCallback, tvSessionManagementModule},
			{sessionStoppedCallback, tvSessionManagementModule}
		});

	agentConnection->setStatusChangedCallback({connectionStatusChanged, tvSessionManagementModule});

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

	// after destroyAgentConnection() we are sure logging is not used and we can safely destroy it
	agentAPI->destroyLogging(logging);

	printf("Exiting...\n");

	return EXIT_SUCCESS;
}
