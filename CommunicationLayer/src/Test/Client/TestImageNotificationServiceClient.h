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
#pragma once

#include "TestData/TestDataImageNotifcation.h"

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/TransportFramework.h>

#include <TVRemoteScreenSDKCommunication/ImageNotificationService/IImageNotificationServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ImageNotificationService/ServiceFactory.h>

#include <iostream>
#include <memory>
#include <string>

namespace TestImageNotificationService
{

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
int TestImageNotificationServiceClient(int /*argc*/, char** /*argv*/)
{
	using namespace TVRemoteScreenSDKCommunication::ImageNotificationService;
	using TestData = TestData<Framework>;
	const std::string LogPrefix = "[ImageNotificationService][Client][fw=" + std::to_string(Framework) + "] ";

	const std::shared_ptr<IImageNotificationServiceClient> client = ServiceFactory::CreateClient<Framework>();
	if (client->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::ImageNotification)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	client->StartClient(TestData::Socket);
	if (client->GetDestination() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	TVRemoteScreenSDKCommunication::CallStatus response{};

	response = client->UpdateImageDefinition(TestData::ComId, TestData::ImageSourceTitle, TestData::Width, TestData::Height);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "UpdateImageDefinition successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "UpdateImageDefinition Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	response = client->NotifiyImageChanged(TestData::ComId, TestData::X, TestData::Y, TestData::Width, TestData::Height);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "NotifyImageChange successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "UpdateImageDefinition Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

} // namespace TestImageNotificationService
