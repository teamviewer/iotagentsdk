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
#include "Client/ImageTestPerformanceClient.h"
#include "Client/InputTestPerformanceClient.h"

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/TransportFramework.h>

#include <iostream>
#include <thread>
#include <unistd.h>

constexpr const char* FTPSToken = "FTPS";
constexpr const char* KTPSToken = "KTPS";
constexpr const char* gRPCToken = "gRPC";
constexpr const char* SocketIOToken = "SocketIO";
constexpr const char* PicturePathToken = "PicturePath";
constexpr const char* TestTimeToken = "TestTime";
constexpr const char Seperator = '=';
constexpr size_t SeperatorSize = 1;
constexpr size_t TokenIsAtTheBeginningIndex = 0;
static_assert(SeperatorSize == 1, "seperator must be a single char");

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
struct SvcConnData;

template<>
struct SvcConnData<TVRemoteScreenSDKCommunication::gRPCTransport>
{
	static constexpr const char* DefaultImageServiceLocation = "/tmp/imageService";
	static constexpr const char* DefaultInputServiceLocation = "/tmp/inputService";
	static constexpr const char* Prefix = "unix://";
};

template<>
struct SvcConnData<TVRemoteScreenSDKCommunication::TCPSocketTransport>
{
	static constexpr const char* DefaultImageServiceLocation = ":9003";
	static constexpr const char* DefaultInputServiceLocation = ":9004";
	static constexpr const char* Prefix = "127.0.0.1";
};

struct KTPS
{
	bool isSet = false;
};

struct FTPS
{
	bool isSet = false;
	std::string picturePath;
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
	CommandLineResult result;

	for (int argumentIndex = 1; argumentIndex < argc; ++argumentIndex)
	{
		const std::string argument(argv[argumentIndex]);

		if (argument == FTPSToken)
		{
			result.ftps.isSet = true;
			continue;
		}

		if (argument == KTPSToken)
		{
			result.ktps.isSet = true;
			continue;
		}

		if (argument == gRPCToken)
		{
			result.testgRPC = true;
		}

		if (argument == SocketIOToken)
		{
			result.testSocketIO = true;
		}

		const size_t picturePathLocation = argument.find(PicturePathToken);

		if (picturePathLocation == TokenIsAtTheBeginningIndex)
		{
			std::string tokenAsString(PicturePathToken);

			if (argument.size() > (picturePathLocation + tokenAsString.size() + SeperatorSize) && argument.at(tokenAsString.size()) == Seperator)
			{
				result.ftps.picturePath = argument.substr(picturePathLocation + tokenAsString.size() + 1);
			}
			continue;
		}

		const size_t testTimeLocation = argument.find(TestTimeToken);
		if (testTimeLocation == TokenIsAtTheBeginningIndex)
		{
			std::string tokenAsString(TestTimeToken);

			if (argument.size() > testTimeLocation + tokenAsString.size() + SeperatorSize && argument.at(tokenAsString.size()) == Seperator)
			{
				std::string secondsAsString = argument.substr(testTimeLocation + tokenAsString.size() + 1);
				try
				{
					result.testTime = std::stoi(secondsAsString);
				}
				catch (const std::exception& e)
				{
					std::cerr << "Exception during converting the test time: " << e.what() << std::endl;
				}
			}
			continue;
		}
	}

	return result;
}

void PrintHelp()
{
	std::cout
		<< "Usage: TVRemoteScreenSDKCommunicationTest_PerformanceTestClient ["
		<< FTPSToken << " " << PicturePathToken << Seperator << "<PathToPicture>] ["
		<< KTPSToken << "] [" << gRPCToken << "|" << SocketIOToken << "] ["
		<< TestTimeToken << Seperator << "<seconds>]" << std::endl
		<< "	" << FTPSToken << ": Frame transmissions per second" << std::endl
		<< "	" << KTPSToken << ": Key transmissions per second" << std::endl
		<< "	" << TestTimeToken << ": Overall runtime of this test in seconds. Default is 60 seconds." << std::endl
		<< "This programm will send the defined set of data to the TVRemoteScreenSDKCommunicationTest_PerformanceTestServer." << std::endl;
}

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
void RunTest(const CommandLineResult& cmdLineResult)
{
	TestImageServicePerformance::StopCondition stopConditionImage;
	stopConditionImage.run = true;
	std::thread imageThread;
	const bool imageTestRequested = cmdLineResult.ftps.isSet && !cmdLineResult.ftps.picturePath.empty();
	if (imageTestRequested)
	{
		imageThread = std::thread([&stopConditionImage, &cmdLineResult]()
		{
			TestImageServicePerformance::TestImageServiceClient<Framework>(
				stopConditionImage,
				std::string(SvcConnData<Framework>::Prefix) +
					std::string(SvcConnData<Framework>::DefaultImageServiceLocation),
				cmdLineResult.ftps.picturePath);
		});
	}

	TestInputServicePerformance::StopCondition stopConditionInput;
	stopConditionInput.run = true;
	std::thread inputThread;
	const bool inputTestRequested = cmdLineResult.ktps.isSet;
	if (inputTestRequested)
	{
		inputThread = std::thread([&stopConditionInput]()
		{
			TestInputServicePerformance::TestInputServiceClient<Framework>(
				stopConditionInput,
				std::string(SvcConnData<Framework>::Prefix) +
					std::string(SvcConnData<Framework>::DefaultInputServiceLocation));
		});
	}

	sleep(cmdLineResult.testTime);

	stopConditionImage.run = false;

	stopConditionInput.run = false;

	if (imageTestRequested)
	{
		imageThread.join();
	}

	if (inputTestRequested)
	{
		inputThread.join();
	}
}

int main(int argc, char** argv)
{
	const CommandLineResult parseResult = ParseCommandLine(argc, argv);

	if (parseResult.ftps.isSet && parseResult.ftps.picturePath.empty())
	{
		std::cerr << "Missing picture path" << std::endl;
		PrintHelp();
		return EXIT_FAILURE;
	}

	if (!parseResult.ftps.isSet && !parseResult.ktps.isSet)
	{
		PrintHelp();
		return EXIT_FAILURE;
	}

	if (parseResult.testgRPC && parseResult.testSocketIO)
	{
		std::cerr << "You can test only one transport at once" << std::endl;
		PrintHelp();
		return EXIT_FAILURE;
	}

	if (parseResult.testgRPC)
	{
#ifdef TV_COMM_ENABLE_GRPC
		RunTest<TVRemoteScreenSDKCommunication::gRPCTransport>(parseResult);
#else
		std::cerr << "gRPC is not supported" << std::endl;
		return EXIT_FAILURE;
#endif
	}
	else if (parseResult.testSocketIO)
	{
#ifdef TV_COMM_ENABLE_PLAIN_SOCKET
		RunTest<TVRemoteScreenSDKCommunication::TCPSocketTransport>(parseResult);
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
}
