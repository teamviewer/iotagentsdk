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

#include "TestData/TestDataInstantSupport.h"

#include <TVRemoteScreenSDKCommunication/InstantSupportService/IInstantSupportServiceServer.h>
#include <TVRemoteScreenSDKCommunication/InstantSupportService/ServiceFactory.h>

#include <iostream>

namespace TestInstantSupportService
{

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
std::shared_ptr<TVRemoteScreenSDKCommunication::InstantSupportService::IInstantSupportServiceServer>
    TestInstantSupportServiceServer()
{
	using namespace TVRemoteScreenSDKCommunication::InstantSupportService;
	using TVRemoteScreenSDKCommunication::CallStatus;
	using TVRemoteScreenSDKCommunication::CallState;
	using TestData = TestData<Framework>;
	const std::string LogPrefix = "[InstantSupportService][Server][fw=" + std::to_string(Framework) + "] ";

	const std::shared_ptr<IInstantSupportServiceServer> server = ServiceFactory::CreateServer<Framework>();
	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::InstantSupport)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}
	auto requestInstantSupportCallback = [LogPrefix](
		const std::string& comId,
		const std::string& accessToken,
		const std::string& name,
		const std::string& group,
		const std::string& description,
		const std::string& sessionCode,
		const std::string& email,
		const IInstantSupportServiceServer::RequestInstantSupportResponseCallback& response)
	{
		std::cout << LogPrefix << "Received request instant support with: "
			<< comId << "(comId), "
			<< accessToken << "(accessToken), "
			<< name << "(name), "
			<< group << "(group), "
			<< description << "(description), "
			<< sessionCode << "(sessionCode)."
			<< email << "(email)." << std::endl;

		if (comId == TestData::ComId
			&& accessToken == TestData::AccessToken
			&& name == TestData::Name
			&& group == TestData::Group
			&& description == TestData::Description
			&& sessionCode == TestData::SessionCode
			&& email == TestData::Email)
		{
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetRequestInstantSupportCallback(requestInstantSupportCallback);

	server->StartServer(TestData::Socket);

	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}

} // namespace TestInstantSupportOutService
