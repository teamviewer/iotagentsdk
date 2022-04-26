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
#include "TestConnectionConfirmationServiceServer.h"

#include "TestData/TestDataConnectionConfirmation.h"

#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/ConnectionData.h>
#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/ServiceFactory.h>

#include <iostream>

namespace TestConnectionConfirmationService
{

std::shared_ptr<IConnectionConfirmationRequestServiceServer> TestConnectionConfirmationRequestServiceServer()
{
	constexpr const char* LogPrefix = "[ConnectionConfirmationRequestService][Server] ";

	using namespace TVRemoteScreenSDKCommunication::ConnectionConfirmationService;

	const std::shared_ptr<IConnectionConfirmationRequestServiceServer> server = ServiceFactory::CreateConnectionConfirmationRequestServiceServer();

	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::ConnectionConfirmationRequest)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	const auto connectionConfirmationRequestCallback = [](
		const std::string& comId,
		TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionType connectionType,
		const IConnectionConfirmationRequestServiceServer::ConnectionConfirmationRequestResponseCallback& response)
	{
		std::cout << LogPrefix << "Received connection confirmation request" << std::endl;

		if (comId == TestData::ComId && connectionType == TestData::ConnectionTypeInstantSupport)
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetRequestConnectionConfirmationCallback(connectionConfirmationRequestCallback);

	server->StartServer(TestData::Socket);

	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}

std::shared_ptr<IConnectionConfirmationResponseServiceServer> TestConnectionConfirmationResponseServiceServer()
{
	constexpr const char* LogPrefix = "[ConnectionConfirmationResponseService][Server] ";

	using namespace TVRemoteScreenSDKCommunication::ConnectionConfirmationService;

	const std::shared_ptr<IConnectionConfirmationResponseServiceServer> server = ServiceFactory::CreateConnectionConfirmationResponseServiceServer();

	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::ConnectionConfirmationResponse)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	const auto connectionConfirmationCallback = [](
		const std::string& comId,
		ConnectionType connectionType,
		ConnectionUserConfirmation confirmation,
		const IConnectionConfirmationResponseServiceServer::ConfirmConnectionRequestResponseCallback& response)
	{
		std::cout << LogPrefix << "Received connection confirmation response" << std::endl;

		if (comId == TestData::ComId
			&& connectionType == TestData::ConnectionTypeInstantSupport
			&& confirmation == TestData::Accepted)
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetConfirmConnectionRequestCallback(connectionConfirmationCallback);

	server->StartServer(TestData::Socket);

	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}

} // namespace TestConnectionConfirmationService
