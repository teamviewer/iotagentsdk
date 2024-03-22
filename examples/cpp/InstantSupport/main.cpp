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
#include <getopt.h>

namespace
{

struct
{
	const char* accessToken = "<your-access-token>";
	const char* name = "Help Me Please!";
	const char* group = "Contacts";
	const char* description = "I need your support!";
	const char* email = "supporter@teamviewer.com";
	const char* sessionCode = "";
} s_requestData;

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
	printf("[IAgentConnection] Status: %s\n", toCString(status));

	if (status == tvagentapi::IAgentConnection::Status::Connected)
	{
		auto instantSupportModule = static_cast<tvagentapi::IInstantSupportModule*>(userdata);
		printf("\tRequesting instant support... {Access token: '%s', group: '%s', email: '%s', name: '%s', description: '%s', sessionCode: '%s'}\n",
			s_requestData.accessToken,
			s_requestData.group,
			s_requestData.email,
			s_requestData.name,
			s_requestData.description,
			s_requestData.sessionCode);

		instantSupportModule->requestInstantSupport(
			s_requestData.accessToken,
			s_requestData.name,
			s_requestData.group,
			s_requestData.description,
			s_requestData.email,
			s_requestData.sessionCode);
	}
}

void instantSupportSessionDataChanged(
	tvagentapi::IInstantSupportModule::SessionData newData,
	void* userdata) noexcept
{
	(void)userdata;

	printf("[IInstantSupportModule] instantSupportSessionDataChanged: ");
	if (newData.state == tvagentapi::IInstantSupportModule::SessionState::Undefined)
	{
		printf("no session data\n");
	}
	else
	{
		// NOTE: SessionData c-strings are only valid during this callback call
		// and have to be copied in order to be retained
		printf(
			"{state: %s, name: %s, description: %s, sessionCode: %s}\n",
			tvagentapi::toCString(newData.state),
			newData.name,
			newData.description,
			newData.sessionCode);
	}
}

void instantSupportRequestError(
	tvagentapi::IInstantSupportModule::RequestErrorCode errorCode,
	void* userdata) noexcept
{
	(void)userdata;

	printf(
		"[IInstantSupportModule] instantSupportRequestError: '%s'\n",
		tvagentapi::toCString(errorCode));
}

void instantSupportConnectionRequested(void* userdata) noexcept
{
	auto instantSupportModule = static_cast<tvagentapi::IInstantSupportModule*>(userdata);

	printf(
		"Incoming Instant Support request. Supporter wants to connect to your machine.\n"
		"Available options:\n"
		" 'a' - Accept connection request\n"
		" 'r' - Reject connection request\n"
		" 't' - Timeout connection request\n"
		"Pick an option (a/r/T): ");

	switch (getchar())
	{
		case 'a':
		case 'A':
			instantSupportModule->acceptConnectionRequest();
			printf("Instant support request accepted\n");
			break;
		case 'r':
		case 'R':
			instantSupportModule->rejectConnectionRequest();
			printf("Instant support request rejected\n");
			break;
		default:
			instantSupportModule->timeoutConnectionRequest();
			printf("Instant support request timed out\n");
			break;
	}
}

void parseArgs(int argc, char* argv[])
{
	constexpr const char* usage =
		"Usage: example_InstantSupport [options...]\n"
		"Make Instant Support request. Options change default fields in request data.\n"
		"\n"
		"Options\n"
		"  -a, --access-token   - request access token\n"
		"  -n, --name           - request caption\n"
		"  -d, --description    - request description\n"
		"  -g, --group          - request group\n"
		"  -e, --email          - request email (optional)\n"
		"  -s, --session-code   - request session-code (optional)\n"
		"\n";

	constexpr struct option longOptions[] =
	{
		{ "access-token",	required_argument, nullptr, 'a' },
		{ "name",			required_argument, nullptr, 'n' },
		{ "group",			required_argument, nullptr, 'g' },
		{ "description",	required_argument, nullptr, 'd' },
		{ "email",			required_argument, nullptr, 'e' },
		{ "session-code",	required_argument, nullptr, 's' },
		{}
	};

	int index = 0;
	int opt;

	while ((opt = getopt_long(argc, argv, "ha:n:g:d:e:s:", longOptions, &index)) != -1)
	{
		switch (opt)
		{
			case 'h':
				printf("%s", usage);
				exit(EXIT_SUCCESS);
				break;
			case 'a':
				s_requestData.accessToken = optarg;
				break;
			case 'n':
				s_requestData.name = optarg;
				break;
			case 'g':
				s_requestData.group = optarg;
				break;
			case 'd':
				s_requestData.description = optarg;
				break;
			case 'e':
				s_requestData.email = optarg;
				break;
			case 's':
				s_requestData.sessionCode = optarg;
				break;
			default:
				fprintf(stderr, "%s", usage);
				exit(EXIT_FAILURE);
		}
	}
}

} // namespace

int main(int argc, char* argv[])
{
	parseArgs(argc, argv);

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

	tvagentapi::IInstantSupportModule* instantSupportModule =
		tvagentapi::getModule<tvagentapi::IInstantSupportModule>(agentConnection);
	if (!instantSupportModule)
	{
		fputs("Failed to get InstantSupportModule\n", stderr);
		return EXIT_FAILURE;
	}
	if (!instantSupportModule->isSupported())
	{
		fputs("InstantSupportModule not supported\n", stderr);
		return EXIT_FAILURE;
	}

	instantSupportModule->setCallbacks({
			{instantSupportSessionDataChanged, nullptr/*userdata*/},
			{instantSupportRequestError, nullptr/*userdata*/},
			{instantSupportConnectionRequested, instantSupportModule}
		});

	agentConnection->setStatusChangedCallback({connectionStatusChanged, instantSupportModule});

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
