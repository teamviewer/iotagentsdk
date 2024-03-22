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

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/TransportFramework.h>

#include <TVRemoteScreenSDKCommunication/RegistrationService/IRegistrationServiceClient.h>
#include <TVRemoteScreenSDKCommunication/RegistrationService/ServiceFactory.h>
#include <TVRemoteScreenSDKCommunication/RegistrationService/ServiceInformation.h>

#include <iostream>

namespace TestRegistrationService
{

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
int TestRegistrationServiceClient(int /*argc*/, char** /*argv*/)
{
	using namespace TVRemoteScreenSDKCommunication::RegistrationService;
	using TestData = TestData<Framework>;
	const std::string LogPrefix = "[RegistrationService][Client][fw=" + std::to_string(Framework) + "] ";
	const std::shared_ptr<IRegistrationServiceClient> client = ServiceFactory::CreateClient<Framework>();
	if (client->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::Registration)
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

	IRegistrationServiceClient::ExchangeVersionResponse exchangeVersionResponse;
	TVRemoteScreenSDKCommunication::CallStatus response;
	IRegistrationServiceClient::DiscoverResponse discoverResponse;

	exchangeVersionResponse = client->ExchangeVersion(TestData::ComVersion);
	if (exchangeVersionResponse.IsOk())
	{
		if (exchangeVersionResponse.versionNumber == TestData::ComVersion)
		{
			std::cout << LogPrefix << "ExchangeVersion successful " << exchangeVersionResponse.versionNumber << std::endl;
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		std::cerr << LogPrefix << "ExchangeVersion Error: " << exchangeVersionResponse.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	discoverResponse = client->Discover(TestData::ComVersion);
	if (discoverResponse.IsOk())
	{
		if (discoverResponse.communicationId == TestData::ComId && discoverResponse.services[0].type == TestData::ServiceType && discoverResponse.services[0].location == TestData::ServiceLocation)
		{
			std::cout << LogPrefix << "Discover successful " << discoverResponse.communicationId <<"(comId)";
			for (const ServiceInformation& serviceInfo : discoverResponse.services)
			{
				std::cout << ", ";
				switch (serviceInfo.type)
				{
					case TVRemoteScreenSDKCommunication::ServiceType::Image:
						std::cout << "Image";
						break;
					case TVRemoteScreenSDKCommunication::ServiceType::Connectivity:
						std::cout << "Connectivity";
						break;
					case TVRemoteScreenSDKCommunication::ServiceType::Input:
						std::cout << "Input";
						break;
					case TVRemoteScreenSDKCommunication::ServiceType::ImageNotification:
						std::cout << "ImageNotification";
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
						break;
				}
				std::cout << serviceInfo.location <<"(loc)";
			}
			std::cout << std::endl;
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		std::cerr << "[Client] Discover Error: " << discoverResponse.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	response = client->Register(TestData::ComId, TestData::ServiceType, TestData::ServiceLocation);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "RegisterService successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "RegisterService Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

} // namespace TestRegistrationService
