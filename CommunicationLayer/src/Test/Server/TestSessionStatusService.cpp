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
#include "TestSessionStatusService.h"

#include "TestData/TestDataSessionStatus.h"

#include <TVRemoteScreenSDKCommunication/SessionStatusService/ServiceFactory.h>

#include <iostream>

namespace TestSessionStatusService
{

constexpr const char* LogPrefix = "[SessionStatusService][Server] ";

std::shared_ptr<TVRemoteScreenSDKCommunication::SessionStatusService::ISessionStatusServiceServer> TestSessionStatusService()
{
	using namespace TVRemoteScreenSDKCommunication::SessionStatusService;
	const std::shared_ptr<ISessionStatusServiceServer> server = ServiceFactory::CreateServer();
	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::SessionStatus)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	const auto rcStarted = [](
		const std::string& comId,
		const TVRemoteScreenSDKCommunication::SessionStatusService::GrabStrategy strategy,
		const ISessionStatusServiceServer::RemoteControllStartedResponseCallback& response)
	{
		std::cout << LogPrefix << "Received RemoteControlStarted with: " << comId << std::endl;
		if (comId == TestData::ComId && strategy == TestData::GrabStrategy)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetRemoteControlStartedCallback(rcStarted);

	const auto rcEnded = [](
		const std::string& comId,
		const ISessionStatusServiceServer::RemoteControllStoppedResponseCallback& response)
	{
		std::cout << LogPrefix << "Received RemoteControlStopped with: " << comId << std::endl;
		if (comId == TestData::ComId)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetRemoteControlStoppedCallback(rcEnded);

	const auto tvSessionStarted = [](
		const std::string& comId,
		int32_t tvSessionID,
		int32_t tvSessionsCount,
		const ISessionStatusServiceServer::RemoteControllStoppedResponseCallback& response)
	{
		std::cout << LogPrefix << "Received TVSessionStarted with: " << comId
			<< " { id: " << tvSessionID << ", count: " << tvSessionsCount << " }" << std::endl;

		if (comId == TestData::ComId &&
			tvSessionID == TestData::DummyTVSessionID &&
			tvSessionsCount == TestData::SessionCountStarted)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetTVSessionStartedCallback(tvSessionStarted);

	const auto tvSessionStopped = [](
		const std::string& comId,
		int32_t tvSessionID,
		int32_t tvSessionsCount,
		const ISessionStatusServiceServer::RemoteControllStoppedResponseCallback& response)
	{
		std::cout << LogPrefix << "Received TVSessionStopped with: " << comId
			<< " { id: " << tvSessionID << ", count: " << tvSessionsCount << " }" << std::endl;

		if (comId == TestData::ComId &&
			tvSessionID == TestData::DummyTVSessionID &&
			tvSessionsCount == TestData::SessionCountStopped)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetTVSessionStoppedCallback(tvSessionStopped);

	server->StartServer(TestData::Socket);
	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}

} // namespace TestSessionStatusService
