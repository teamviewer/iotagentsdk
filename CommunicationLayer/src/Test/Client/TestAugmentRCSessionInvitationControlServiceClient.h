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

#include <TVRemoteScreenSDKCommunication/AugmentRCSessionInvitationService/IAugmentRCSessionInvitationControlServiceClient.h>
#include <TVRemoteScreenSDKCommunication/AugmentRCSessionInvitationService/ControlServiceFactory.h>

#include <iostream>

namespace TestAugmentRCSessionInvitationControlService
{

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
int TestAugmentRCSessionInvitationControlServiceClient(int /*argc*/, char ** /*argv*/)
{
	using namespace TVRemoteScreenSDKCommunication::AugmentRCSessionInvitationService;
	using TestData = TestData<Framework>;
	const std::string LogPrefix =
			"[AugmentRCSessionInvitationControlService][Client][fw=" + std::to_string(Framework) + "] ";

	const std::shared_ptr<IAugmentRCSessionInvitationControlServiceClient> client = ControlServiceFactory::CreateClient<Framework>();

	if (client->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::AugmentRCSessionInvitationControl)
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

	TVRemoteScreenSDKCommunication::CallStatus startListeningResponse = client->StartListening(TestData::ComId);
	if (startListeningResponse.IsOk())
	{
		std::cout << LogPrefix << "StartListening successful" << std::endl;
	} else
	{
		std::cerr << LogPrefix << "StartListening Error: " << startListeningResponse.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	TVRemoteScreenSDKCommunication::CallStatus cancelReceivedInvitationResponse = client->CancelReceivedInvitation(
			TestData::ComId, CancellationReason::ClosedDialog);
	if (cancelReceivedInvitationResponse.IsOk())
	{
		std::cout << LogPrefix << "CancelReceivedInvitation successful" << std::endl;
	} else
	{
		std::cerr << LogPrefix << "CancelReceivedInvitation Error: " << cancelReceivedInvitationResponse.errorMessage
				  << std::endl;
		return EXIT_FAILURE;
	}

	TVRemoteScreenSDKCommunication::CallStatus stopListeningResponse = client->StopListening(TestData::ComId);
	if (stopListeningResponse.IsOk())
	{
		std::cout << LogPrefix << "StopListening successful" << std::endl;
	} else
	{
		std::cerr << LogPrefix << "StopListening Error: " << stopListeningResponse.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

} // namespace TestAccessControlService
