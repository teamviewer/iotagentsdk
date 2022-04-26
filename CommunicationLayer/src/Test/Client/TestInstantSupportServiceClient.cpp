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
#include "TestInstantSupportServiceClient.h"

#include "TestData/TestDataInstantSupport.h"

#include <TVRemoteScreenSDKCommunication/InstantSupportService/IInstantSupportServiceClient.h>
#include <TVRemoteScreenSDKCommunication/InstantSupportService/ServiceFactory.h>

#include <memory>
#include <iostream>
#include <string>

namespace TestInstantSupportService
{

constexpr const char* LogPrefix = "[InstantSupportService][Client] ";

int TestInstantSupportServiceClient(int /*argc*/, char** /*argv*/)
{
	using namespace TVRemoteScreenSDKCommunication::InstantSupportService;

	TVRemoteScreenSDKCommunication::CallStatus response = TVRemoteScreenSDKCommunication::CallState::Failed;
	const std::shared_ptr<IInstantSupportServiceClient> client = ServiceFactory::CreateClient();

	if (client->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::InstantSupport)
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

	response = client->RequestInstantSupport(TestData::ComId, TestData::AccessToken, TestData::Name, TestData::Group, TestData::Description, TestData::SessionCode, TestData::Email);

	if (response.IsOk())
	{
		std::cout << LogPrefix << "RequestInstantSupport successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "RequestInstantSupport Failed" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

} // namespace TestInstantSupportService
