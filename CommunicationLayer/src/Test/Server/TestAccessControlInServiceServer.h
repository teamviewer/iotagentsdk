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

#include "TestData/TestDataAccessControlIn.h"

#include <TVRemoteScreenSDKCommunication/AccessControlService/IAccessControlInServiceServer.h>
#include <TVRemoteScreenSDKCommunication/AccessControlService/InServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/ServiceType.h>

#include <iostream>
#include <memory>

namespace TestAccessControlInService
{

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
std::shared_ptr<TVRemoteScreenSDKCommunication::AccessControlService::IAccessControlInServiceServer> TestAccessControlInServiceServer()
{
	using TVRemoteScreenSDKCommunication::CallStatus;
	using TVRemoteScreenSDKCommunication::CallState;
	using namespace TVRemoteScreenSDKCommunication::AccessControlService;
	using TestData = TestData<Framework>;
	const std::string LogPrefix = "[AccessControlInService][Server][fw=" + std::to_string(Framework) + "] ";

	const std::shared_ptr<IAccessControlInServiceServer> server = InServiceFactory::CreateServer<Framework>();

	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::AccessControlIn)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	const auto processConfirmationReply = [LogPrefix](
		const std::string& comId,
		AccessControl feature,
		bool confirmed,
		const IAccessControlInServiceServer::ConfirmationReplyResponseCallback& response)
	{
		std::cout
			<< LogPrefix
			<< "Received ConfirmationReply request with: "
			<< comId << "(comId), ";

		switch (feature)
		{
			case AccessControl::FileTransfer:
				std::cout << "AccessControl::FileTransfer, ";
				break;
			default:
				std::cerr << LogPrefix << "Undefined AccessControl value" << std::endl;
				exit(EXIT_FAILURE);
		}

		std::string confirmedToString = confirmed ? "true" : "false";
		std::cout << "(confirmed) " << confirmedToString << std::endl;

		if (comId == TestData::ComId && feature == TestData::feature && confirmed == TestData::confirmed)
		{
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetConfirmationReplyCallback(processConfirmationReply);

	const auto processGetAccess = [LogPrefix](
		const std::string& comId,
		AccessControl feature,
		const IAccessControlInServiceServer::GetAccessResponseCallback& response)
	{
		std::cout
			<< LogPrefix
			<< "Received GetAccess request with: "
			<< comId << "(comId) ";

		switch (feature)
		{
			case AccessControl::FileTransfer:
				std::cout << "AccessControl::FileTransfer ";
				break;
			default:
				std::cerr << LogPrefix << "Undefined AccessControl value" << std::endl;
				exit(EXIT_FAILURE);
		}

		std::cout << std::endl;

		if (feature == TestData::feature)
		{
			response(CallStatus::Ok, TestData::access);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetGetAccessCallback(processGetAccess);

	const auto processSetAccess = [LogPrefix](
		const std::string& comId,
		AccessControl feature,
		Access access,
		const IAccessControlInServiceServer::SetAccessResponseCallback& response)
	{
		std::cout
			<< LogPrefix
			<< "Received SetAccess request with: "
			<< comId << "(comId), ";

		switch (feature)
		{
			case AccessControl::FileTransfer:
				std::cout << "AccessControl::FileTransfer, ";
				break;
			default:
				std::cerr << LogPrefix << "Undefined AccessControl value" << std::endl;
				exit(EXIT_FAILURE);
		}

		switch (access)
		{
			case Access::Allowed:
				std::cout << "Access::Allowed";
				break;
			case Access::AfterConfirmation:
				std::cout << "Access::AfterConfirmation";
				break;
			case Access::Denied:
				std::cout << "Access::Denied";
				break;
			default:
				std::cerr << LogPrefix << "Undefined Access value" << std::endl;
				exit(EXIT_FAILURE);
		}

		std::cout << std::endl;

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

	server->SetSetAccessCallback(processSetAccess);

	server->StartServer(TestData::Socket);
	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}

} // namespace TestAccessControlInService
