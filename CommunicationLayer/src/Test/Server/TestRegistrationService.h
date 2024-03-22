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

#include "TestData/TestDataRegistration.h"

#include <TVRemoteScreenSDKCommunication/RegistrationService/IRegistrationServiceServer.h>
#include <TVRemoteScreenSDKCommunication/RegistrationService/ServiceFactory.h>

#include <iostream>

namespace TestRegistrationService
{

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
std::shared_ptr<TVRemoteScreenSDKCommunication::RegistrationService::IRegistrationServiceServer> TestRegistrationService()
{
	using namespace TVRemoteScreenSDKCommunication::RegistrationService;
	using TVRemoteScreenSDKCommunication::CallStatus;
	using TVRemoteScreenSDKCommunication::CallState;
	using TestData = TestData<Framework>;
	const std::string LogPrefix = "[RegistrationService][Server][fw=" + std::to_string(Framework) + "] ";
	const std::shared_ptr<IRegistrationServiceServer> server = ServiceFactory::CreateServer<Framework>();
	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::Registration)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	const auto exchangeVersionRequest = [LogPrefix](const std::string& receivedVersion, const IRegistrationServiceServer::ExchangeVersionResponseCallback& response)
	{
		std::cout << LogPrefix << "Received ExchangeVersion with " << receivedVersion << std::endl;
		if (receivedVersion == TestData::ComVersion)
		{
			response(TestData::ComVersion, CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetExchangeVersionCallback(exchangeVersionRequest);


	const auto discoverRequest = [LogPrefix](const std::string& communicationVersion, const IRegistrationServiceServer::DiscoverResponseCallback& response)
	{
		std::cout << LogPrefix << "Received DiscoverRequest with " << communicationVersion << std::endl;

		if (communicationVersion == TestData::ComVersion)
		{
			std::vector<ServiceInformation> services;

			ServiceInformation serviceInfo{};
			serviceInfo.location = TestData::ServiceLocation;
			serviceInfo.type = TestData::ServiceType;
			services.push_back(serviceInfo);

			response(TestData::ComId, services, CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetDiscoverCallback(discoverRequest);

	const auto registerRequest = [LogPrefix](const std::string& comId, TVRemoteScreenSDKCommunication::ServiceType id, const std::string& location, const IRegistrationServiceServer::RegisterResponseCallback& response)
	{
		std::cout << LogPrefix << "Received RegisterRequest with " << comId << "(comId), ";
		switch (id)
		{
			case TVRemoteScreenSDKCommunication::ServiceType::Image:
				std::cout << "Image";
				break;
			case TVRemoteScreenSDKCommunication::ServiceType::ImageNotification:
				std::cout << "ImageNotification";
				break;
			case TVRemoteScreenSDKCommunication::ServiceType::Connectivity:
				std::cout << "Connectivity";
				break;
			case TVRemoteScreenSDKCommunication::ServiceType::Input:
				std::cout << "Input";
				break;
			case TVRemoteScreenSDKCommunication::ServiceType::Registration:
				std::cout << "Registration";
				break;
			case TVRemoteScreenSDKCommunication::ServiceType::SessionControl:
				std::cout << "SessionControl";
				break;
			case TVRemoteScreenSDKCommunication::ServiceType::SessionStatus:
				std::cout << "SessionStatus";
				break;
			case TVRemoteScreenSDKCommunication::ServiceType::AccessControlIn:
				std::cout << "AccessControlIn";
				break;
			case TVRemoteScreenSDKCommunication::ServiceType::AccessControlOut:
				std::cout << "AccessControlOut";
				break;
			case TVRemoteScreenSDKCommunication::ServiceType::InstantSupport:
				std::cout << "InstantSupport";
				break;
			case TVRemoteScreenSDKCommunication::ServiceType::ViewGeometry:
				std::cout << "ViewGeometry";
				break;
			case TVRemoteScreenSDKCommunication::ServiceType::InstantSupportNotification:
				std::cout << "InstantSupportNotification";
				break;
			case TVRemoteScreenSDKCommunication::ServiceType::ChatIn:
				std::cout << "ChatIn";
				break;
			case TVRemoteScreenSDKCommunication::ServiceType::ChatOut:
				std::cout << "ChatOut";
				break;
			case TVRemoteScreenSDKCommunication::ServiceType::ConnectionConfirmationRequest:
				std::cout << "ConnectionConfirmationRequest";
				break;
			case TVRemoteScreenSDKCommunication::ServiceType::ConnectionConfirmationResponse:
				std::cout << "ConnectionConfirmationResponse";
				break;
			case TVRemoteScreenSDKCommunication::ServiceType::AugmentRCSessionInvitationControl:
				std::cout << "AugmentRCSessionInvitationControl";
				break;
			case TVRemoteScreenSDKCommunication::ServiceType::AugmentRCSessionInvitationConsumer:
				std::cout << "AugmentRCSessionInvitationConsumer";
				break;
			case TVRemoteScreenSDKCommunication::ServiceType::Unknown:
				std::cout << "Unknown";
		}
		std::cout << ", " << location << "(loc), " << std::endl;

		if (comId == TestData::ComId && id == TestData::ServiceType && location == TestData::ServiceLocation)
		{
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetRegisterCallback(registerRequest);

	server->StartServer(TestData::Socket);
	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}

} // namespace TestRegistrationService
