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

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/TransportFramework.h>

#include <TVRemoteScreenSDKCommunication/ChatService/IChatOutServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ChatService/OutServiceFactory.h>

#include <iostream>

namespace TestChatOutService
{

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
std::shared_ptr<TVRemoteScreenSDKCommunication::ChatService::IChatOutServiceServer> TestChatOutServiceServer()
{
	using namespace TVRemoteScreenSDKCommunication;
	using namespace TVRemoteScreenSDKCommunication::ChatService;
	using TestData = TestData<Framework>;
	const std::string LogPrefix = "[ChatOutService][Server][fw=" + std::to_string(Framework) + "] ";

	const std::shared_ptr<IChatOutServiceServer> server = OutServiceFactory::CreateServer<Framework>();
	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::ChatOut)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	const auto processChatCreatedRequestCallback = [LogPrefix](
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
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetChatCreatedCallback(processChatCreatedRequestCallback);

	const auto processChatsRemovedRequestCallback = [LogPrefix](
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
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetChatsRemovedCallback(processChatsRemovedRequestCallback);

	const auto processReceivedMessageRequestCallback = [LogPrefix](
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
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetReceivedMessagesCallback(processReceivedMessageRequestCallback);

	const auto processMessageSentRequestCallback = [LogPrefix](
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
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetMessageSentCallback(processMessageSentRequestCallback);

	const auto processMessageNotSentRequestCallback = [LogPrefix](
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
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetMessageNotSentCallback(processMessageNotSentRequestCallback);

	const auto processLoadedMessageRequestCallback = [LogPrefix](
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
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetLoadedMessagesCallback(processLoadedMessageRequestCallback);

	const auto processDeletedHistoryRequestCallback = [LogPrefix](
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
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetDeletedHistoryCallback(processDeletedHistoryRequestCallback);

	const auto processClosedChatRequestCallback = [LogPrefix](
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
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetClosedChatCallback(processClosedChatRequestCallback);

	server->StartServer(TestData::Socket);
	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}

} // namespace TestChatOutService
