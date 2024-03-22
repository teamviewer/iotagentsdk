//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2024 TeamViewer Germany GmbH                                     //
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

#include "TestData/TestDataAugmentRCSessionInvitationControl.h"

#include <TVRemoteScreenSDKCommunication/AugmentRCSessionInvitationService/IAugmentRCSessionInvitationControlServiceServer.h>
#include <TVRemoteScreenSDKCommunication/AugmentRCSessionInvitationService/ControlServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/ServiceType.h>

#include <iostream>
#include <memory>
#include <TVRemoteScreenSDKCommunication/AugmentRCSessionInvitationService/IAugmentRCSessionInvitationControlServiceServer.h>

namespace TestAugmentRCSessionInvitationControlService
{

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
std::shared_ptr<TVRemoteScreenSDKCommunication::AugmentRCSessionInvitationService::IAugmentRCSessionInvitationControlServiceServer>
TestAugmentRCSessionInvitationControlServiceServer()
{
	using TVRemoteScreenSDKCommunication::CallStatus;
	using TVRemoteScreenSDKCommunication::CallState;
	using namespace TVRemoteScreenSDKCommunication::AugmentRCSessionInvitationService;
	using TestData = TestData<Framework>;
	const std::string LogPrefix =
			"[AugmentRCSessionInvitationControlService][Server][fw=" + std::to_string(Framework) + "] ";

	const std::shared_ptr<IAugmentRCSessionInvitationControlServiceServer> server = ControlServiceFactory::CreateServer<Framework>();

	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::AugmentRCSessionInvitationControl)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	const auto processStartListening = [LogPrefix](
			const std::string& comId,
			const IAugmentRCSessionInvitationControlServiceServer::StartListeningResponseCallback& response)
	{
		std::cout
				<< LogPrefix
				<< "Received StartListening request with: "
				<< comId << "(comId)" << std::endl;

		if (comId == TestData::ComId)
		{
			response(CallStatus::Ok);
		} else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetStartListeningCallback(processStartListening);

	const auto processCancelReceivedInvitation = [LogPrefix](
			const std::string& comId,
			CancellationReason cancellationReason,
			const IAugmentRCSessionInvitationControlServiceServer::CancelReceivedInvitationResponseCallback& response)
	{
		std::cout
				<< LogPrefix
				<< "Received StopListening request with: "
				<< comId << "(comId) ";

		switch (cancellationReason)
		{
			case CancellationReason::PressedCancelButton:
				std::cout << "CancellationReason::PressedCancelButton";
				break;
			case CancellationReason::ClosedDialog:
				std::cout << "CancellationReason::ClosedDialog";
				break;
			case CancellationReason::Invalid:
				std::cout << "CancellationReason::Invalid";
				break;
			default:
				std::cerr << LogPrefix << "Undefined CancellationReason" << std::endl;
				exit(EXIT_FAILURE);
		}
		std::cout << std::endl;

		if (comId == TestData::ComId)
		{
			response(CallStatus::Ok);
		} else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetCancelReceivedInvitationCallback(processCancelReceivedInvitation);

	const auto processStopListening = [LogPrefix](
			const std::string& comId,
			const IAugmentRCSessionInvitationControlServiceServer::StopListeningResponseCallback& response)
	{
		std::cout
				<< LogPrefix
				<< "Received StopListening request with: "
				<< comId << "(comId) " << std::endl;

		if (comId == TestData::ComId)
		{
			response(CallStatus::Ok);
		} else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetStopListeningCallback(processStopListening);

	server->StartServer(TestData::Socket);
	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}

} // namespace TestAccessControlService
