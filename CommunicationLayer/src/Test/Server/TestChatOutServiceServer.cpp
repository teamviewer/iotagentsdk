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
#include "TestChatOutServiceServer.h"

#include "TestData/TestDataChatOut.h"

#include <TVRemoteScreenSDKCommunication/ChatService/OutServiceFactory.h>

#include <string>
#include <memory>
#include <stdio.h>
#include <iostream>
#include <thread>

namespace TestChatOutService
{

constexpr const char* LogPrefix = "[ChatOutService][Server] ";

using namespace TVRemoteScreenSDKCommunication::ChatService;

std::shared_ptr<IChatOutServiceServer> TestChatOutServiceServer()
{
	const std::shared_ptr<IChatOutServiceServer> server = OutServiceFactory::CreateServer();
	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::ChatOut)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	const auto processChatCreatedRequestCallback = [](
		const std::string& comId,
		std::string chatId,
		std::string title,
		ChatType chatType,
		uint32_t chatTypeId,
		const IChatOutServiceServer::ChatCreatedResponseCallback& response)
	{
		std::cout << LogPrefix << "Received ChatCreated with: "
			<< comId << "(comId), "
			<< chatId << "(chatId) "
			<< title << "(title) ";

			switch (chatType)
			{
				case ChatType::Machine:
					std::cout << "Machine" << std::endl;
					break;
				case ChatType::Session:
					std::cout << "Session" << std::endl;
					break;
				default:
					std::cerr << LogPrefix << "Undefined ChatType value" << std::endl;
					exit(EXIT_FAILURE);
					break;
			}
			std::cout << "(chatType) "
			<< chatTypeId << "(chatTypeId)"
			<< std::endl;

		if (comId == TestData::ComId &&
			chatId == TestData::ChatId &&
			title == TestData::ChatTitle &&
			chatType == TestData::ChatType &&
			chatTypeId == TestData::ChatTypeId)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetProcessChatCreatedRequestCallback(processChatCreatedRequestCallback);

	const auto processChatsRemovedRequestCallback = [](
		const std::string& comId,
		std::vector<std::string> chatIds,
		const IChatOutServiceServer::ChatsRemovedResponseCallback& response)
	{
		std::cout << LogPrefix << "Received ChatsRemoved with: "
			<< comId << "(comId), "
			<< chatIds[0] << "(chatId)"
			<< std::endl;

		if (comId == TestData::ComId && chatIds[0] == TestData::ChatId)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetProcessChatsRemovedRequestCallback(processChatsRemovedRequestCallback);

	const auto processReceivedMessageRequestCallback = [](
		const std::string& comId,
		std::vector<ReceivedMessage> receivedMessage,
		const IChatOutServiceServer::ReceivedMessagesResponseCallback& response)
	{
		std::cout << LogPrefix << "Received ReceivedMessages with: "
			<< comId << "(comId), "
			<< receivedMessage[0].chatId << "(chatId), "
			<< receivedMessage[0].messageId << "(messageId), "
			<< receivedMessage[0].content << "(content), "
			<< std::hex << receivedMessage[0].timeStamp << "(timeStamp), " << std::dec
			<< receivedMessage[0].sender << "(sender)"
			<< std::endl;

		if (comId == TestData::ComId &&
			receivedMessage[0].messageId == TestData::MessageId &&
			receivedMessage[0].content == TestData::Content &&
			receivedMessage[0].timeStamp == TestData::TimeStamp)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetProcessReceivedMessagesRequestCallback(processReceivedMessageRequestCallback);

	const auto processMessageSentRequestCallback = [](
		const std::string& comId,
		uint32_t localId,
		std::string messageId,
		uint64_t timeStamp,
		const IChatOutServiceServer::MessageSentResponseCallback& response)
	{
		std::cout << LogPrefix << "Received MessageSent with: "
			<< comId << "(comId), "
			<< localId << "(localId), "
			<< messageId << "(messageId), "
			<< std::hex << timeStamp << "(timeStamp)" << std::dec
			<< std::endl;

		if (comId == TestData::ComId && localId == TestData::LocalId && messageId == TestData::MessageId && timeStamp == TestData::TimeStamp)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetProcessMessageSentRequestCallback(processMessageSentRequestCallback);

	const auto processMessageNotSentRequestCallback = [](
		const std::string& comId,
		uint32_t localId,
		const IChatOutServiceServer::MessageNotSentResponseCallback& response)
	{
		std::cout << LogPrefix << "Received MessageNotSent with: "
			<< comId << "(comId), "
			<< localId << "(localId)"
			<< std::endl;

		if (comId == TestData::ComId && localId == TestData::LocalId)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetProcessMessageNotSentRequestCallback(processMessageNotSentRequestCallback);

	const auto processLoadedMessageRequestCallback = [](
		const std::string& comId,
		std::vector<ReceivedMessage> messages,
		bool hasMore,
		const IChatOutServiceServer::LoadedMessagesResponseCallback& response)
	{
		std::cout << LogPrefix << "Received LoadedMessages with: "
			<< comId << "(comId), "
			<< messages[0].chatId << "(chatId), "
			<< messages[0].messageId << "(messageId), "
			<< messages[0].content << "(content), "
			<< std::hex << messages[0].timeStamp << "(timeStamp), " << std::dec
			<< messages[0].sender << "(sender), "
			<< (hasMore ? "true" : "fasle") << "(hasMore)"
			<< std::endl;

		if (comId == TestData::ComId &&
			messages[0].messageId == TestData::MessageId &&
			messages[0].content == TestData::Content &&
			messages[0].timeStamp == TestData::TimeStamp &&
			hasMore == false)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetProcessLoadedMessagesRequestCallback(processLoadedMessageRequestCallback);

	const auto processDeletedHistoryRequestCallback = [](
		const std::string& comId,
		std::string chatId,
		const IChatOutServiceServer::DeletedHistoryResponseCallback& response)
	{
		std::cout << LogPrefix << "Received DeletedHistory with: "
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
	server->SetProcessDeletedHistoryRequestCallback(processDeletedHistoryRequestCallback);

	const auto processClosedChatRequestCallback = [](
		const std::string& comId,
		std::string chatId,
		const IChatOutServiceServer::ClosedChatResponseCallback& response)
	{
		std::cout << LogPrefix << "Received ClosedChat with: "
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
	server->SetProcessClosedChatRequestCallback(processClosedChatRequestCallback);

	server->StartServer(TestData::Socket);
	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}

} // namespace TestChatOutService
