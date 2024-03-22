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

#include "TestData/TestDataSessionControl.h"

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/ServiceType.h>

#include <TVRemoteScreenSDKCommunication/SessionControlService/ISessionControlServiceServer.h>
#include <TVRemoteScreenSDKCommunication/SessionControlService/ServiceFactory.h>

#include <iostream>

namespace TestSessionControlService
{

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
std::shared_ptr<TVRemoteScreenSDKCommunication::SessionControlService::ISessionControlServiceServer> TestSessionControlService()
{
	using namespace TVRemoteScreenSDKCommunication::SessionControlService;
	using TVRemoteScreenSDKCommunication::CallStatus;
	using TVRemoteScreenSDKCommunication::CallState;
	using TestData = TestData<Framework>;
	const std::string LogPrefix = "[SessionControlService][Server][fw=" + std::to_string(Framework) + "] ";

	const std::shared_ptr<ISessionControlServiceServer> server = ServiceFactory::CreateServer<Framework>();

	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::SessionControl)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	const auto processChangeMode = [LogPrefix](
		const std::string& comId,
		ControlMode controlMode,
		const ISessionControlServiceServer::ChangeControlModeResponseCallback& response)
	{
		std::cout
			<< LogPrefix
			<< "Received change mode with: "
			<< comId << "(comId), ";

		switch (controlMode)
		{
			case ControlMode::FullControl:
				std::cout << "FullControl";
				break;
			case ControlMode::ScreenSharing:
				std::cout << "ScreenSharing";
				break;
			case ControlMode::Disable:
				std::cout << "Disable";
				break;
			case ControlMode::Unknown:
				std::cout << "Unknown";
				break;
		}

		std::cout << std::endl;

		if (comId == TestData::ComId && controlMode == TestData::Mode)
		{
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr  << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetChangeControlModeCallback(processChangeMode);

	const auto processCloseRC = [LogPrefix](
		const std::string& comId,
		const ISessionControlServiceServer::CloseRcResponseCallback& response)
	{
		std::cout
			<< LogPrefix
			<< "Received Close RC with: "
			<< comId << "(comId), "
			<< std::endl;

		if (comId == TestData::ComId)
		{
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr  << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetCloseRcCallback(processCloseRC);

	const auto getRunningTVSessions = [LogPrefix](
		const std::string& comId,
		const ISessionControlServiceServer::GetRunningTVSessionsResponseCallback& response)
	{
		std::cout
			<< LogPrefix
			<< "Received GetRunningTVSessions with: "
			<< comId << "(comId), "
			<< std::endl;

		if (comId == TestData::ComId)
		{
			std::vector<int32_t> testSessions{TestData::RunningTVSessions()};
			response(CallStatus::Ok, testSessions);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetGetRunningTVSessionsCallback(getRunningTVSessions);

	server->StartServer(TestData::Socket);
	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}

} // namespace TestSessionControlService
