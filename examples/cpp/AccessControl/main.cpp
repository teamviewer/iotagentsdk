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

#include <initializer_list>

namespace
{

struct
{
	tvagentapi::IAccessControlModule::Access fileTransferAccess =
		tvagentapi::IAccessControlModule::Access::Denied;
	tvagentapi::IAccessControlModule::Access remoteViewAccess =
		tvagentapi::IAccessControlModule::Access::Denied;
	tvagentapi::IAccessControlModule::Access remoteControlAccess =
		tvagentapi::IAccessControlModule::Access::Denied;
	tvagentapi::IAccessControlModule::Access screenRecordingAccess =
		tvagentapi::IAccessControlModule::Access::Denied;
} s_accessData;

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

	if (status != tvagentapi::IAgentConnection::Status::Connected)
	{
		return;
	}
	using Feature = tvagentapi::IAccessControlModule::Feature;
	using Access = tvagentapi::IAccessControlModule::Access;

	auto accessControlModule = static_cast<tvagentapi::IAccessControlModule*>(userdata);

	printf("Getting current access...\n");

	for (const auto feature:
		{Feature::FileTransfer, Feature::RemoteView, Feature::RemoteControl, Feature::ScreenRecording})
	{
		tvagentapi::IAccessControlModule::Access access = Access::Denied;
		if (!accessControlModule->getAccess(feature, access))
		{
			fprintf(
				stderr, "Failed to retrieve %s access\n",
				tvagentapi::toCString(feature));
		}
		else
		{
			printf(
				"\t%s: %s\n",
				tvagentapi::toCString(feature),
				tvagentapi::toCString(access));
		}
	}

	printf("Setting access...\n\t{%s => %s, %s => %s, %s => %s, %s => %s}\n",
		tvagentapi::toCString(Feature::FileTransfer),
		tvagentapi::toCString(s_accessData.fileTransferAccess),
		tvagentapi::toCString(Feature::RemoteView),
		tvagentapi::toCString(s_accessData.remoteViewAccess),
		tvagentapi::toCString(Feature::RemoteControl),
		tvagentapi::toCString(s_accessData.remoteControlAccess),
		tvagentapi::toCString(Feature::ScreenRecording),
		tvagentapi::toCString(s_accessData.screenRecordingAccess));

	if (!accessControlModule->setAccess(
			Feature::FileTransfer,
			s_accessData.fileTransferAccess))
	{
		fprintf(stderr, "Failed to change access for file transfer");
	}

	if (!accessControlModule->setAccess(
			Feature::RemoteView,
			s_accessData.remoteViewAccess))
	{
		fprintf(stderr, "Failed to change access for remote view");
	}

	if (!accessControlModule->setAccess(
			Feature::RemoteControl,
			s_accessData.remoteControlAccess))
	{
		fprintf(stderr, "Failed to change access for remote control");
	}

	if (!accessControlModule->setAccess(
			Feature::ScreenRecording,
			s_accessData.screenRecordingAccess))
	{
		fprintf(stderr, "Failed to change access for screen recording");
	}
}

void accessChanged(
	tvagentapi::IAccessControlModule::Feature feature,
	tvagentapi::IAccessControlModule::Access access,
	void* userdata) noexcept
{
	(void)userdata;

	printf(
		"[IAccessControlModule] accessChanged: %s => %s\n",
		tvagentapi::toCString(feature),
		tvagentapi::toCString(access));
}

void accessConfirmationRequested(
	tvagentapi::IAccessControlModule::Feature feature,
	void* userdata) noexcept
{
	auto accessControlModule = static_cast<tvagentapi::IAccessControlModule*>(userdata);

	printf(
		"Incoming access confirmation request. Access to %s requested.\n"
		"Confirm Access control request? (y/N): ",
		tvagentapi::toCString(feature));

	switch (getchar())
	{
		case 'y':
		case 'Y':
			accessControlModule->acceptAccessRequest(feature);
			printf("Access confirmation request accepted\n");
			break;
		default:
			accessControlModule->rejectAccessRequest(feature);
			printf("Access confirmation request rejected\n");
			break;
	}
}

void parseArgs(int argc, char* argv[])
{
	auto printUsage = [](decltype(stdout) stream)
	{
		constexpr const char* usageFormat =
			R"(Usage: example_AccessControl [options...]
Set access control per feature. Options change defaults in access control.

Usage: example_AccessControl [options...]

Options:
	-f, --file-transfer    - set initial file transfer access
	-v, --remote-view      - set initial remote view access
	-c, --remote-control   - set initial remote control access
	-r, --screen-recording - set initial screen recording access

Acceptable access values:
	%d - Allowed
	%d - After confirmation
	%d - Denied

By default everything is denied.
)";

		fprintf(
			stream,
			usageFormat,
			static_cast<int>(tvagentapi::IAccessControlModule::Access::Allowed),
			static_cast<int>(tvagentapi::IAccessControlModule::Access::AfterConfirmation),
			static_cast<int>(tvagentapi::IAccessControlModule::Access::Denied));
	};

	constexpr struct option longOptions[] =
	{
		{ "file-transfer",		required_argument, nullptr, 'f' },
		{ "remote-view",		required_argument, nullptr, 'v' },
		{ "remote-control",		required_argument, nullptr, 'c' },
		{ "screen-recording",	required_argument, nullptr, 'r' },
		{}
	};

	int index = 0;
	int opt;

	auto toAccess = [printUsage](const char* arg) -> tvagentapi::IAccessControlModule::Access
	{
		switch (*arg)
		{
			case '0':
				return tvagentapi::IAccessControlModule::Access::Allowed;
			case '1':
				return tvagentapi::IAccessControlModule::Access::AfterConfirmation;
			case '2':
				return tvagentapi::IAccessControlModule::Access::Denied;
		}

		printUsage(stderr);
		exit(EXIT_FAILURE);
	};

	while ((opt = getopt_long(argc, argv, "hf:v:c:r:", longOptions, &index)) != -1)
	{
		switch (opt)
		{
			case 'h':
				printUsage(stdout);
				exit(EXIT_SUCCESS);
			case 'f':
				s_accessData.fileTransferAccess = toAccess(optarg);
				break;
			case 'v':
				s_accessData.remoteViewAccess = toAccess(optarg);
				break;
			case 'c':
				s_accessData.remoteControlAccess = toAccess(optarg);
				break;
			case 'r':
				s_accessData.screenRecordingAccess = toAccess(optarg);
				break;
			default:
				printUsage(stderr);
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
	tvagentapi::ILogging* logging = agentAPI->createFileLogging("example_AccessControl.log");
	if (!logging)
	{
		fputs("Failed to start file logging\n", stderr);
		return EXIT_FAILURE;
	}

	// connect tvagentapi sdk to IoT Agent
	// We pass logging to AgentConnection, but we still manage its lifetime and must pair it with IAgentAPI::destroyLogging()
	// logging object will be used internally, and we may only release it (in this case by calling IAgentAPI::destroyLogging()
	// after call to agentAPI->destroyAgentConnection();
	tvagentapi::IAgentConnection* agentConnection =
		agentAPI->createAgentConnection(logging);
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

	tvagentapi::IAccessControlModule* accessControlModule =
		tvagentapi::getModule<tvagentapi::IAccessControlModule>(agentConnection);
	if (!accessControlModule)
	{
		fputs("Failed to get AccessControlModule\n", stderr);
		return EXIT_FAILURE;
	}
	if (!accessControlModule->isSupported())
	{
		fputs("AccessControlModule not supported\n", stderr);
		return EXIT_FAILURE;
	}

	accessControlModule->setCallbacks({
		{accessChanged, nullptr/*userdata*/},
		{accessConfirmationRequested, accessControlModule},
	});

	agentConnection->setStatusChangedCallback({connectionStatusChanged, accessControlModule});

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
