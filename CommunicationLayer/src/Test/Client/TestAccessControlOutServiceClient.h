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

#include "TestData/TestDataAccessControlOut.h"

#include <TVRemoteScreenSDKCommunication/AccessControlService/IAccessControlOutServiceClient.h>
#include <TVRemoteScreenSDKCommunication/AccessControlService/OutServiceFactory.h>

#include <iostream>

namespace TestAccessControlOutService
{

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
int TestAccessControlOutServiceClient(int /*argc*/, char** /*argv*/)
{
	using namespace TVRemoteScreenSDKCommunication::AccessControlService;
	using TestData = TestData<Framework>;
	const std::string LogPrefix = "[AccessControlInService][Client][fw=" + std::to_string(Framework) + "] ";

	TVRemoteScreenSDKCommunication::CallStatus response;
	const std::shared_ptr<IAccessControlOutServiceClient> client = OutServiceFactory::CreateClient<Framework>();

	if (client->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::AccessControlOut)
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

	response = client->AskForConfirmation(TestData::ComId, TestData::feature, TestData::timeout);

	if (response.IsOk())
	{
		std::cout << LogPrefix << "AskForConfirmation successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "AskForConfirmation Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	response = client->NotifyChange(TestData::ComId, TestData::feature, TestData::access);

	if (response.IsOk())
	{
		std::cout << LogPrefix << "NotifyChange successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "NotifyChange Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

} // namespace TestAccessControlOutService
