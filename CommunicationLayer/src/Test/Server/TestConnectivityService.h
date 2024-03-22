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

#include "TestData/TestDataConnectivity.h"

#include <TVRemoteScreenSDKCommunication/ConnectivityService/IConnectivityServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ConnectivityService/ServiceFactory.h>

#include <iostream>

namespace TestConnectivityService
{

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
std::shared_ptr<TVRemoteScreenSDKCommunication::ConnectivityService::IConnectivityServiceServer> TestConnectivityService()
{
	using TVRemoteScreenSDKCommunication::CallStatus;
	using TVRemoteScreenSDKCommunication::CallState;
	using namespace TVRemoteScreenSDKCommunication::ConnectivityService;
	using TestData = TestData<Framework>;
	const std::string LogPrefix = "[ConnectivityService][Server][fw=" + std::to_string(Framework) + "] ";

	const std::shared_ptr<IConnectivityServiceServer> server = ServiceFactory::CreateServer<Framework>();
	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::Connectivity)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	const auto isAvailableRequest = [LogPrefix](const std::string& comId, const IConnectivityServiceServer::IsAvailableResponseCallback& response)
	{
		std::cout << LogPrefix << "Received IsAvailable with " << comId << "(comId)" << std::endl;
		if (comId != TestData::ComId)
		{
			exit(EXIT_FAILURE);
		}
		response(CallStatus::Ok);
	};
	server->SetIsAvailableCallback(isAvailableRequest);

	const auto disconnectRequest = [LogPrefix](const std::string& comId, const IConnectivityServiceServer::DisconnectResponseCallback& response)
	{
		std::cout << LogPrefix << "Received DisconnectRequest with " << comId << "(comId)" << std::endl;
		if (comId != TestData::ComId)
		{
			exit(EXIT_FAILURE);
		}
		response(CallStatus::Ok);
	};
	server->SetDisconnectCallback(disconnectRequest);

	server->StartServer(TestData::Socket);
	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}

} // namespace TestConnectivityService
