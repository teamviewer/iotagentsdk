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

#include <TVRemoteScreenSDKCommunication/AccessControlService/IAccessControlOutServiceServer.h>
#include <TVRemoteScreenSDKCommunication/AccessControlService/OutServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/ServiceType.h>

#include <iostream>
#include <memory>

namespace TestAccessControlOutService
{

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
std::shared_ptr<TVRemoteScreenSDKCommunication::AccessControlService::IAccessControlOutServiceServer>
	TestAccessControlOutServiceServer()
{
	using TVRemoteScreenSDKCommunication::CallState;
	using TVRemoteScreenSDKCommunication::CallStatus;
	using namespace TVRemoteScreenSDKCommunication::AccessControlService;
	using TestData = TestData<Framework>;
	const std::string LogPrefix = "[AccessControlOutService][Server][fw=" + std::to_string(Framework) + "] ";

	const std::shared_ptr<IAccessControlOutServiceServer> server = OutServiceFactory::CreateServer<Framework>();
	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::AccessControlOut)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	const auto getConfirmation = [LogPrefix](
		const std::string& comId,
		AccessControl feature,
		uint32_t timeout,
		const IAccessControlOutServiceServer::AskForConfirmationResponseCallback& response)
	{
		std::cout << LogPrefix << "Received AccessControl AskForConfirmation with: " << comId << "(comId), ";

		switch (feature)
		{
			case AccessControl::FileTransfer:
				std::cout << "AccessControl::FileTransfer, ";
				break;
			default:
				std::cerr << LogPrefix << "Undefined AccessControl value" << std::endl;
				exit(EXIT_FAILURE);
				break;
		}

		std::cout
			<< timeout
			<< "(timeout in seconds)"
			<< std::endl;

		if (comId == TestData::ComId && feature == TestData::feature && timeout == TestData::timeout)
		{
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetAskForConfirmationCallback(getConfirmation);

	const auto notifyChange = [LogPrefix](
		const std::string& comId,
		AccessControl feature,
		Access access,
		const IAccessControlOutServiceServer::NotifyChangeResponseCallback& response)
	{
		std::cout << LogPrefix << "Received Notify AccessControl Change with: " << comId << "(comId), ";

		switch (feature)
		{
			case AccessControl::FileTransfer:
				std::cout << "AccessControl::FileTransfer, ";
				break;
			default:
				std::cerr << LogPrefix << "Undefined AccessControl value" << std::endl;
				exit(EXIT_FAILURE);
				break;
		}

		switch (access)
		{
			case Access::Allowed:
				std::cout << " Access::Allowed" << std::endl;
				break;
			case Access::AfterConfirmation:
				std::cout << " Access::AfterConfirmation" << std::endl;
				break;
			case Access::Denied:
				std::cout << " Access::Denied" << std::endl;
				break;
			default:
				std::cerr << LogPrefix << "Undefined Access value" << std::endl;
				exit(EXIT_FAILURE);
				break;
		}

		if (comId == TestData::ComId && feature == TestData::feature && access == TestData::access)
		{
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetNotifyChangeCallback(notifyChange);

	server->StartServer(TestData::Socket);
	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}

} // namespace TestAccessControlOutService
