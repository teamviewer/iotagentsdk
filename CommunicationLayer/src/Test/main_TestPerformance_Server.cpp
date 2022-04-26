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
constexpr const char* TestTimeToken = "TestTime";
constexpr const char* DefaultImageServiceLocation = "/tmp/imageService";
constexpr const char* DefaultInputServiceLocation = "/tmp/inputService";
constexpr const char* GrpcUnixDefintion = "unix://";
constexpr const char* ResultFileFolder = "./";
constexpr const char* ImageResultFile = "imageResults.csv";
constexpr const char* InputResultFile = "inputResults.csv";
constexpr const char Seperator = '=';
constexpr size_t SeperatorSize = 1;
constexpr size_t TokenIsAtTheBeginningIndex = 0;
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
		<< "Usage: TVRemoteScreenSDKCommunicationTest_PerformanceTestServer [" << FTPSToken << "] [" << KTPSToken << "] [" << TestTimeToken << Seperator << "<seconds>]" << std::endl
		<< "	" << FTPSToken << ": Frame transmissions per second" << std::endl
		<< "	" << KTPSToken << ": Key transmissions per second" << std::endl
		<< "	" << TestTimeToken << ": Overall runtime of this test in seconds. Default is 60 seconds." << std::endl
		<< "This programm will receive the defined set of data from the TVRemoteScreenSDKCommunicationTest_PerformanceTestClient." << std::endl
		<< "The amount of received data will be saved after the test in files next to the program." << std::endl;
}

int main(int argc, char** argv)
{
	std::shared_ptr<TVRemoteScreenSDKCommunication::ImageService::IImageServiceServer> imageService;
	std::shared_ptr<TVRemoteScreenSDKCommunication::InputService::IInputServiceServer> inputService;
	const CommandLineResult result = ParseCommandLine(argc, argv);

	std::vector<uint64_t> imageResults;
	TestImageServicePerformance::SharedInformation imageInformation;
	std::thread imageCounterThread;
	ThreadStopper imageThreadStopper;
	if (result.ftps.isSet)
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

		imageService = TestImageServicePerformance::TestImageService(imageInformation, std::string(GrpcUnixDefintion) + std::string(DefaultImageServiceLocation));
	}

	std::vector<uint64_t> inputResults;
	TestInputServicePerformance::SharedInformation inputInformation;
	std::thread inputCounterThread;
	ThreadStopper inputThreadStopper;
	if (result.ktps.isSet)
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

		inputService = TestInputServicePerformance::TestInputService(inputInformation, std::string(GrpcUnixDefintion) + std::string(DefaultInputServiceLocation));
	}

	if (imageService == nullptr && inputService == nullptr)
	{
		PrintHelp();
		return EXIT_FAILURE;
	}

	sleep(result.testTime);

	if (imageService)
	{
		imageThreadStopper.run = false;

		imageService->StopServer();
		imageCounterThread.join();
		WriteResult(imageResults, std::string(ResultFileFolder) + std::string(ImageResultFile));
	}
	if (inputService)
	{
		inputThreadStopper.run = false;

		inputService->StopServer();
		inputCounterThread.join();
		WriteResult(inputResults, std::string(ResultFileFolder) + std::string(InputResultFile));
	}
	return EXIT_SUCCESS;
}
