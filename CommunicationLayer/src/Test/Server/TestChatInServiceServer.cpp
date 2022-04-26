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
#include "TestChatInServiceServer.h"

#include "TestData/TestDataChatIn.h"

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/ServiceType.h>

#include <TVRemoteScreenSDKCommunication/ChatService/InServiceFactory.h>

#include <iostream>

namespace TestChatInService
{

constexpr const char* LogPrefix = "[ChatInService][Server] ";
using namespace TVRemoteScreenSDKCommunication::ChatService;

std::shared_ptr<IChatInServiceServer> TestChatInServiceServer()
{
	const std::shared_ptr<IChatInServiceServer> server = InServiceFactory::CreateServer();

	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::ChatIn)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	const auto processObtainChatsCallback = [](
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

			response(chats, TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetProcessObtainChatsCallback(processObtainChatsCallback);

	const auto processSelectChatRequestCallback = [](
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
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetProcessSelectChatCallback(processSelectChatRequestCallback);

	const auto processSendMessageRequestCallback = [](
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
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetProcessSendMessageCallback(processSendMessageRequestCallback);

	const auto processLoadMessagesRequestCallback = [](
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
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetProcessLoadMessagesCallback(processLoadMessagesRequestCallback);

	const auto processDeleteHistoryRequestCallback = [](
		const std::string& comId,
		const IChatInServiceServer::SendMessageResponseCallback& response)
	{
		std::cout << LogPrefix << "Received DeleteHistory request with: "
			<< comId << "(comId)"
			<< std::endl;

		if (comId == TestData::ComId)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetProcessDeleteHistoryCallback(processDeleteHistoryRequestCallback);

	const auto processDeleteChatRequestCallback = [](
		const std::string& comId,
		const IChatInServiceServer::SendMessageResponseCallback& response)
	{
		std::cout << LogPrefix << "Received DeleteChat request with: "
			<< comId << "(comId)"
			<< std::endl;

		if (comId == TestData::ComId)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetProcessDeleteChatCallback(processDeleteChatRequestCallback);

	server->StartServer(TestData::Socket);
	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}
} // namespace TestChatInService
