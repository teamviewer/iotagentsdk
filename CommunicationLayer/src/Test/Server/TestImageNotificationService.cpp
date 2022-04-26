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
#include "TestImageNotificationService.h"

#include "TestData/TestDataImageNotifcation.h"

#include <TVRemoteScreenSDKCommunication/ImageNotificationService/ServiceFactory.h>

#include <cstdio>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace TestImageNotificationService
{

constexpr const char* LogPrefix = "[ImageNotificationService][Server] ";

std::shared_ptr<TVRemoteScreenSDKCommunication::ImageNotificationService::IImageNotificationServiceServer> TestImageNotificationService()
{
	using namespace TVRemoteScreenSDKCommunication::ImageNotificationService;
	const std::shared_ptr<IImageNotificationServiceServer> server = ServiceFactory::CreateServer();
	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::ImageNotification)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	const auto processImageChangedFunction = [](
		const std::string& comId,
		const std::string& imageSourceTitle,
		int32_t width,
		int32_t height,
		const IImageNotificationServiceServer::UpdateImageDefinitionResponseCallback& response)
	{
		std::cout
			<< LogPrefix
			<< "Received Announce with: "
			<< comId << "(comId), "
			<< imageSourceTitle
			<< "(title), "
			<< width << "(w), " << height << "(h), "
			<< std::endl;

		if (comId == TestData::ComId
			&& imageSourceTitle == TestData::ImageSourceTitle
			&& width == TestData::Width
			&& height == TestData::Height)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr  << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetUpdateImageDefinitionCallback(processImageChangedFunction);

	const auto processImageUpdate = [](
		const std::string& comId,
		int32_t x,
		int32_t y,
		int32_t width,
		int32_t height,
		const IImageNotificationServiceServer::NotifyImageChangedResponseCallback& response)
	{
		std::cout
			<< LogPrefix
			<< "Received ImageUpdate with: "
			<< comId << "(comId), "
			<< x << "(x), "
			<< y << "(y), "
			<< width << "(w), "
			<< height << "(h)"
			<< std::endl;

		if (comId == TestData::ComId && x == TestData::X && y ==TestData::Y && width == TestData::Width && height == TestData::Height)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr  << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetNotifyImageChangedCallback(processImageUpdate);


	server->StartServer(TestData::Socket);
	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr  << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}
} // namespace TestImageNotificationService
