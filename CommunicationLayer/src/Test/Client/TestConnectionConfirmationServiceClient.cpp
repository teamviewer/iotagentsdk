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
#include "TestConnectionConfirmationServiceClient.h"

#include "TestData/TestDataConnectionConfirmation.h"

#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/IConnectionConfirmationRequestServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/IConnectionConfirmationResponseServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/ServiceFactory.h>

#include <memory>
#include <iostream>
#include <string>

namespace TestConnectionConfirmationService
{

int TestConnectionConfirmationRequestServiceClient(int /*argc*/, char** /*argv*/)
{
	constexpr const char* LogPrefix = "[ConnectionConfirmationRequestService][Client] ";

	using namespace TVRemoteScreenSDKCommunication::ConnectionConfirmationService;

	TVRemoteScreenSDKCommunication::CallStatus response = TVRemoteScreenSDKCommunication::CallState::Failed;
	const std::shared_ptr<IConnectionConfirmationRequestServiceClient> client = ServiceFactory::CreateConnectionConfirmationRequestServiceClient();

	if (client->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::ConnectionConfirmationRequest)
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

	response = client->RequestConnectionConfirmation(TestData::ComId, TestData::ConnectionTypeInstantSupport);

	if (response.IsOk())
	{
		std::cout << LogPrefix << "RequestConnectionConfirmation successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "RequestConnectionConfirmation Failed" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int TestConnectionConfirmationResponseServiceClient(int /*argc*/, char** /*argv*/)
{
	constexpr const char* LogPrefix = "[ConnectionConfirmationResponseService][Client] ";

	using namespace TVRemoteScreenSDKCommunication::ConnectionConfirmationService;

	TVRemoteScreenSDKCommunication::CallStatus response = TVRemoteScreenSDKCommunication::CallState::Failed;
	const std::shared_ptr<IConnectionConfirmationResponseServiceClient> client = ServiceFactory::CreateConnectionConfirmationResponseServiceClient();

	if (client->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::ConnectionConfirmationResponse)
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

	response = client->ConfirmConnectionRequest(
		TestData::ComId,
		TestData::ConnectionTypeInstantSupport,
		TestData::Accepted);

	if (response.IsOk())
	{
		std::cout << LogPrefix << "ResponseConnectionConfirmation successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "ResponseConnectionConfirmation Failed" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

} // namespace TestConnectionConfirmationService
