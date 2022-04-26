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
#include "TestInstantSupportNotificationServiceClient.h"

#include "TestData/TestDataInstantSupport.h"

#include <TVRemoteScreenSDKCommunication/InstantSupportService/IInstantSupportNotificationServiceClient.h>
#include <TVRemoteScreenSDKCommunication/InstantSupportService/ServiceFactory.h>

#include <memory>
#include <iostream>
#include <string>

namespace TestInstantSupportNotificationService
{

constexpr const char* LogPrefix = "[InstantSupportNotificationService][Client] ";

int TestInstantSupportNotificationServiceClient(int /*argc*/, char** /*argv*/)
{
	using namespace TVRemoteScreenSDKCommunication::InstantSupportService;

	TVRemoteScreenSDKCommunication::CallStatus response = TVRemoteScreenSDKCommunication::CallState::Failed;
	const std::shared_ptr<IInstantSupportNotificationServiceClient> client = ServiceFactory::CreateInstantSupportNotificationServiceClient();

	if (client->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::InstantSupportNotification)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	client->StartClient(TestInstantSupportService::TestData::Socket);

	if (client->GetDestination() != TestInstantSupportService::TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	response = client->NotifyInstantSupportError(TestInstantSupportService::TestData::ComId, TestInstantSupportService::TestData::InvalidToken);

	if (response.IsOk())
	{
		std::cout << LogPrefix << "NotifyInstantSupportError successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "NotifyInstantSupportError failed" << std::endl;
		return EXIT_FAILURE;
	}

	response = client->NotifyInstantSupportModified(TestInstantSupportService::TestData::ComId, TestInstantSupportService::TestData::Data);

	if (response.IsOk())
	{
		std::cout << LogPrefix << "NotifyInstantSupportModified successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "NotifyInstantSupportModified failed" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

} // namespace TestInstantSupportNotificationService
