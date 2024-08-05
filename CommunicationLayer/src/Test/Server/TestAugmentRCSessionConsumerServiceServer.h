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

#include "TestData/TestDataAugmentRCSessionConsumer.h"

#include <TVRemoteScreenSDKCommunication/AugmentRCSessionService/IAugmentRCSessionConsumerServiceServer.h>
#include <TVRemoteScreenSDKCommunication/AugmentRCSessionService/ConsumerServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/ServiceType.h>

#include <iostream>
#include <memory>
#include <TVRemoteScreenSDKCommunication/AugmentRCSessionService/IAugmentRCSessionConsumerServiceServer.h>

namespace TestAugmentRCSessionConsumerService
{

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
std::shared_ptr<TVRemoteScreenSDKCommunication::AugmentRCSessionService::IAugmentRCSessionConsumerServiceServer>
TestAugmentRCSessionConsumerServiceServer()
{
	using TVRemoteScreenSDKCommunication::CallStatus;
	using TVRemoteScreenSDKCommunication::CallState;
	using namespace TVRemoteScreenSDKCommunication::AugmentRCSessionService;
	using TestData = TestData<Framework>;
	const std::string LogPrefix = "[AugmentRCSessionConsumerService][Server][fw=" + std::to_string(Framework) + "] ";

	const std::shared_ptr<IAugmentRCSessionConsumerServiceServer> server = ConsumerServiceFactory::CreateServer<Framework>();

	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::AugmentRCSessionConsumer)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	const auto processReceivedResponse = [LogPrefix](
		const std::string& comId,
		const std::string& invitationUrl,
		const IAugmentRCSessionConsumerServiceServer::ReceivedInvitationResponseCallback& response)
	{
		std::cout
			<< LogPrefix
			<< "Received StartListening request with: "
			<< comId << "(comId)" << std::endl;

		if (comId == TestData::ComId && invitationUrl == TestData::InvitationUrl)
		{
			response(CallStatus::Ok);
		} else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetReceivedInvitationCallback(processReceivedResponse);

	server->StartServer(TestData::Socket);
	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}

} // namespace TestAccessConsumerService
