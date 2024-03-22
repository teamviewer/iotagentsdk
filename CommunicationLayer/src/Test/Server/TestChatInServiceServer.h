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

#include "TestData/TestDataChatIn.h"

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/TransportFramework.h>

#include <TVRemoteScreenSDKCommunication/ChatService/IChatInServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ChatService/InServiceFactory.h>

#include <iostream>

namespace TestChatInService
{

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
std::shared_ptr<TVRemoteScreenSDKCommunication::ChatService::IChatInServiceServer> TestChatInServiceServer()
{
	using namespace TVRemoteScreenSDKCommunication;
	using namespace TVRemoteScreenSDKCommunication::ChatService;
	using TestData = TestData<Framework>;
	const std::string LogPrefix = "[ChatInService][Server][fw=" + std::to_string(Framework) + "] ";

	const std::shared_ptr<IChatInServiceServer> server = InServiceFactory::CreateServer<Framework>();

	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::ChatIn)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	const auto processObtainChatsCallback = [LogPrefix](
		const std::string& comId,
		const IChatInServiceServer::ObtainChatsResponseCallback& response)
	{
		std::cout << LogPrefix << "Received ObtainChats request with: " << comId << "(comId)" << std::endl;

		if (comId == TestData::ComId)
		{
			std::vector<ChatInfo> chats;

			ChatInfo chatInfo;
			chatInfo.chatId = TestData::ChatId;
			chatInfo.title = TestData::ChatTitle;
			chatInfo.chatType = TestData::ChatType;
			chatInfo.chatTypeId = TestData::ChatTypeId;
			chatInfo.chatState = TestData::ChatState;
			chats.push_back(chatInfo);

			response(chats, CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetObtainChatsCallback(processObtainChatsCallback);

	const auto processSelectChatRequestCallback = [LogPrefix](
		const std::string& comId,
		std::string chatId,
		const IChatInServiceServer::SelectChatResponseCallback& response)
	{
		std::cout << LogPrefix << "Received SelectChat request with: "
			<< comId << "(comId), "
			<< chatId << "(chatId)"
			<< std::endl;

		if (comId == TestData::ComId && chatId == TestData::ChatId)
		{
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetSelectChatCallback(processSelectChatRequestCallback);

	const auto processSendMessageRequestCallback = [LogPrefix](
		const std::string& comId,
		uint32_t localId,
		std::string content,
		const IChatInServiceServer::SendMessageResponseCallback& response)
	{
		std::cout << LogPrefix << "Received SendMessage request with: "
			<< comId << "(comId), "
			<< localId << "(localId), "
			<< content << "(content)"
			<< std::endl;

		if (comId == TestData::ComId && localId == TestData::LocalId && content == TestData::Content)
		{
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetSendMessageCallback(processSendMessageRequestCallback);

	const auto processLoadMessagesRequestCallback = [LogPrefix](
		const std::string& comId,
		uint32_t count,
		std::string messageId,
		const IChatInServiceServer::SendMessageResponseCallback& response)
	{
		std::cout << LogPrefix << "Received LoadMessages request with: "
			<< comId << "(comId), "
			<< count << "(count), "
			<< messageId << "(messageId)"
			<< std::endl;

		if (comId == TestData::ComId && count == TestData::Count && messageId == TestData::MessageId)
		{
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetLoadMessagesCallback(processLoadMessagesRequestCallback);

	const auto processDeleteHistoryRequestCallback = [LogPrefix](
		const std::string& comId,
		const IChatInServiceServer::SendMessageResponseCallback& response)
	{
		std::cout << LogPrefix << "Received DeleteHistory request with: "
			<< comId << "(comId)"
			<< std::endl;

		if (comId == TestData::ComId)
		{
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetDeleteHistoryCallback(processDeleteHistoryRequestCallback);

	const auto processDeleteChatRequestCallback = [LogPrefix](
		const std::string& comId,
		const IChatInServiceServer::SendMessageResponseCallback& response)
	{
		std::cout << LogPrefix << "Received DeleteChat request with: "
			<< comId << "(comId)"
			<< std::endl;

		if (comId == TestData::ComId)
		{
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetDeleteChatCallback(processDeleteChatRequestCallback);

	server->StartServer(TestData::Socket);
	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}

} // namespace TestChatInService
