//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2021 TeamViewer Germany GmbH                                     //
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
#include "Server/ImageTestPerformanceServer.h"
#include "Server/InputTestPerformanceServer.h"

#include <fstream>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <atomic>

constexpr const char* FTPSToken = "FTPS";
constexpr const char* KTPSToken = "KTPS";
constexpr const char* gRPCToken = "gRPC";
constexpr const char* SocketIOToken = "SocketIO";
constexpr const char* TestTimeToken = "TestTime";
constexpr const char* ResultFileFolder = "./";
constexpr const char Seperator = '=';
constexpr size_t SeperatorSize = 1;
constexpr size_t TokenIsAtTheBeginningIndex = 0;

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
struct SvcTestData;

template<>
struct SvcTestData<TVRemoteScreenSDKCommunication::gRPCTransport>
{
	static constexpr const char* DefaultImageServiceLocation = "/tmp/imageService";
	static constexpr const char* DefaultInputServiceLocation = "/tmp/inputService";
	static constexpr const char* Prefix = "unix://";
	static constexpr const char* ImageResultFile = "gRPCImageResults.csv";
	static constexpr const char* InputResultFile = "gRPCInputResults.csv";
};

template<>
struct SvcTestData<TVRemoteScreenSDKCommunication::TCPSocketTransport>
{
	static constexpr const char* DefaultImageServiceLocation = "127.0.0.1:9003";
	static constexpr const char* DefaultInputServiceLocation = "127.0.0.1:9004";
	static constexpr const char* Prefix = "tcp+tv://";
	static constexpr const char* ImageResultFile = "TCPSocketImageResults.csv";
	static constexpr const char* InputResultFile = "TCPSocketInputResults.csv";
};

static_assert(SeperatorSize == 1, "seperator must be a single char");

struct KTPS
{
	bool isSet = false;
};

struct FTPS
{
	bool isSet = false;
};

struct CommandLineResult
{
	int testTime = 60; // seconds
	KTPS ktps;
	FTPS ftps;
	bool testgRPC = false;
	bool testSocketIO = false;
};

CommandLineResult ParseCommandLine(int argc, char** argv)
{
	CommandLineResult parseResult;

	for (int argumentIndex = 1; argumentIndex < argc; ++argumentIndex)
	{
		const std::string argument(argv[argumentIndex]);

		if (argument == FTPSToken)
		{
			parseResult.ftps.isSet = true;
		}

		if (argument == KTPSToken)
		{
			parseResult.ktps.isSet = true;
		}

		if (argument == gRPCToken)
		{
			parseResult.testgRPC = true;
		}

		if (argument == SocketIOToken)
		{
			parseResult.testSocketIO = true;
		}

		const size_t testTimeLocation = argument.find(TestTimeToken);

		if (testTimeLocation == TokenIsAtTheBeginningIndex)
		{
			const std::string tokenAsString(TestTimeToken);

			if (argument.size() > testTimeLocation + tokenAsString.size() + SeperatorSize && argument.at(tokenAsString.size()) == Seperator)
			{
				std::string secondsAsString = argument.substr(testTimeLocation + tokenAsString.size() + 1);
				try
				{
					parseResult.testTime = std::stoi(secondsAsString);
				}
				catch (const std::exception& e)
				{
					std::cerr << "Exception during converting the test time: " << e.what() << std::endl;
				}
			}
			continue;
		}
	}

	return parseResult;
}

struct ThreadStopper
{
	std::atomic_bool run{false};
};

void WriteResult(const std::vector<uint64_t>& results, const std::string& path)
{
	std::ofstream resultFile;
	resultFile.open(path);
	if (!resultFile.is_open())
	{
		std::cerr << "Could not open result result file: " << path << std::endl;
		return;
	}

	if (results.size() >= 1)
	{
		resultFile << std::to_string(results[0]) << std::endl;
		for (size_t resultCounter = 1; resultCounter < results.size(); ++resultCounter)
		{
			resultFile << "," << std::to_string(results[resultCounter]) << std::endl;
		}
		resultFile.close();
	}
}

void PrintHelp()
{
	std::cout
		<< "Usage: TVRemoteScreenSDKCommunicationTest_PerformanceTestServer [" << FTPSToken << "] [" << KTPSToken
			<< "] [" << gRPCToken << "|" << SocketIOToken << "] [" << TestTimeToken << Seperator << "<seconds>]" << std::endl
		<< "	" << FTPSToken << ": Frame transmissions per second" << std::endl
		<< "	" << KTPSToken << ": Key transmissions per second" << std::endl
		<< "	" << TestTimeToken << ": Overall runtime of this test in seconds. Default is 60 seconds." << std::endl
		<< "This programm will receive the defined set of data from the TVRemoteScreenSDKCommunicationTest_PerformanceTestClient." << std::endl
		<< "The amount of received data will be saved after the test in files next to the program." << std::endl;
}

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
void RunTestServer(const CommandLineResult& cmdLineResult)
{
	std::shared_ptr<TVRemoteScreenSDKCommunication::ImageService::IImageServiceServer> imageService;
	std::shared_ptr<TVRemoteScreenSDKCommunication::InputService::IInputServiceServer> inputService;

	std::vector<uint64_t> imageResults;
	TestImageServicePerformance::SharedInformation imageInformation;
	std::thread imageCounterThread;
	ThreadStopper imageThreadStopper;
	if (cmdLineResult.ftps.isSet)
	{
		imageThreadStopper.run = true;
		const auto countingLogic = [&imageInformation, &imageThreadStopper, &imageResults]()
		{
			while (imageThreadStopper.run)
			{
				const uint64_t currentCounterValue = imageInformation.counter.exchange(0);
				imageResults.push_back(currentCounterValue);
				sleep(1);
			}
		};

		imageCounterThread = std::thread(countingLogic);

		imageService = TestImageServicePerformance::TestImageService<Framework>(
			imageInformation,
			std::string(SvcTestData<Framework>::Prefix) +
				std::string(SvcTestData<Framework>::DefaultImageServiceLocation));
	}

	std::vector<uint64_t> inputResults;
	TestInputServicePerformance::SharedInformation inputInformation;
	std::thread inputCounterThread;
	ThreadStopper inputThreadStopper;
	if (cmdLineResult.ktps.isSet)
	{
		inputThreadStopper.run = true;
		const auto countingLogic = [&inputInformation, &inputThreadStopper, &inputResults]()
		{
			while (inputThreadStopper.run)
			{
				const uint64_t currentCounterValue = inputInformation.counter.exchange(0);
				inputResults.push_back(currentCounterValue);
				sleep(1);
			}
		};

		inputCounterThread = std::thread(countingLogic);

		inputService = TestInputServicePerformance::TestInputService<Framework>(
			inputInformation,
			std::string(SvcTestData<Framework>::Prefix) +
				std::string(SvcTestData<Framework>::DefaultInputServiceLocation));
	}

	if (imageService == nullptr && inputService == nullptr)
	{
		std::cerr << "Error: none of the test services started.\n\n";
		PrintHelp();
		exit(EXIT_FAILURE);
	}

	sleep(cmdLineResult.testTime);

	if (imageService)
	{
		imageThreadStopper.run = false;

		imageService->StopServer();
		imageCounterThread.join();
		WriteResult(imageResults, std::string(ResultFileFolder) +
			std::string(SvcTestData<Framework>::ImageResultFile));
	}
	if (inputService)
	{
		inputThreadStopper.run = false;

		inputService->StopServer();
		inputCounterThread.join();
		WriteResult(inputResults, std::string(ResultFileFolder) +
			std::string(SvcTestData<Framework>::InputResultFile));
	}
}

int main(int argc, char** argv)
{
	const CommandLineResult result = ParseCommandLine(argc, argv);
	if (result.testgRPC && result.testSocketIO)
	{
		std::cerr << "You can test only one transport at once" << std::endl;
		PrintHelp();
		return EXIT_FAILURE;
	}

	if (result.testgRPC)
	{
#ifdef TV_COMM_ENABLE_GRPC
		RunTestServer<TVRemoteScreenSDKCommunication::gRPCTransport>(result);
#else
		std::cerr << "gRPC is not supported" << std::endl;
		return EXIT_FAILURE;
#endif
	}
	else if (result.testSocketIO)
	{
#ifdef TV_COMM_ENABLE_PLAIN_SOCKET
		RunTestServer<TVRemoteScreenSDKCommunication::TCPSocketTransport>(result);
#else
		std::cerr << "SocketIO is not supported" << std::endl;
		return EXIT_FAILURE;
#endif
	}
	else
	{
		std::cerr << "Choose eiter gRPC or SocketIO to test" << std::endl;
		PrintHelp();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
