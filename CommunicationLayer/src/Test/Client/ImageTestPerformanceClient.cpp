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
#include "ImageTestPerformanceClient.h"

#include <TVRemoteScreenSDKCommunication/ImageService/IImageServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ImageService/ServiceFactory.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <unistd.h>

namespace TestImageServicePerformance
{

constexpr const char* LogPrefix = "[ImageService][Client] ";
constexpr const char* ComId = "comId";
constexpr int32_t X = 0;
constexpr int32_t Y = 0;
constexpr int32_t Width = 100;
constexpr int32_t Height = 100;

int TestImageServiceClient(StopCondition& stopCondition, const std::string& location, const std::string& picturePath)
{
	using namespace TVRemoteScreenSDKCommunication::ImageService;
	const std::shared_ptr<IImageServiceClient> client = ServiceFactory::CreateClient();

	client->StartClient(location);

	std::string picture;

	{
		std::ifstream file(picturePath.c_str(), std::ios::binary | std::ios::ate);

		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		picture.resize(size, '\0');
		file.read(&picture.front(), size);
	}

	int errorCounter = 0;

	while (stopCondition.run)
	{
		const TVRemoteScreenSDKCommunication::CallStatus response = client->UpdateImage(ComId, X, Y, Width, Height, picture);
		if (response.IsOk() == false)
		{
			if (errorCounter > 10)
			{
				std::cerr << "Response faild because: " << response.errorMessage << std::endl;
				return EXIT_FAILURE;
			}
			++errorCounter;
			sleep(1);
		}
		else
		{
			errorCounter = 0;
		}
	}

	return EXIT_SUCCESS;
}

} // namespace TestImageServicePerformance
