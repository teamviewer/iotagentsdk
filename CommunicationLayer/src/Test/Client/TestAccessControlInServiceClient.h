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

#include "TestData/TestDataAccessControlIn.h"

#include <TVRemoteScreenSDKCommunication/AccessControlService/IAccessControlInServiceClient.h>
#include <TVRemoteScreenSDKCommunication/AccessControlService/InServiceFactory.h>

#include <iostream>

namespace TestAccessControlInService
{

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
int TestAccessControlInServiceClient(int /*argc*/, char** /*argv*/)
{
	using namespace TVRemoteScreenSDKCommunication::AccessControlService;
	using TestData = TestData<Framework>;
	const std::string LogPrefix = "[AccessControlInService][Client][fw=" + std::to_string(Framework) + "] ";

	const std::shared_ptr<IAccessControlInServiceClient> client = InServiceFactory::CreateClient<Framework>();

	if (client->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::AccessControlIn)
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

	TVRemoteScreenSDKCommunication::CallStatus confirmationReplyResponse = client->ConfirmationReply(TestData::ComId, TestData::feature, TestData::confirmed);
	if (confirmationReplyResponse.IsOk())
	{
		std::cout << LogPrefix << "ConfirmationReply successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "ConfirmationReply Error: " << confirmationReplyResponse.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	IAccessControlInServiceClient::GetAccessResponse getAccessResponse = client->GetAccess(TestData::ComId, TestData::feature);
	if (getAccessResponse.IsOk() && getAccessResponse.access == TestData::access)
	{
		std::cout << LogPrefix << "GetAccess successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "GetAccess Error: " << getAccessResponse.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	TVRemoteScreenSDKCommunication::CallStatus setAccessResponse = client->SetAccess(TestData::ComId, TestData::feature, TestData::access);
	if (setAccessResponse.IsOk())
	{
		std::cout << LogPrefix << "SetAccess successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "SetAccess Error: " << setAccessResponse.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

} // namespace TestAccessControlInService
