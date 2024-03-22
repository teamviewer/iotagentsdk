//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2022 TeamViewer Germany GmbH                                     //
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
#include "ChatModule.h"

#include <TVAgentAPIPrivate/CommunicationChannel.h>

#include "AgentConnection/AgentConnection.h"
#include "AsyncOperation/IDispatcher.h"
#include "Utils/CallbackUtils.h"
#include "Utils/DispatcherUtils.h"
#include "ModuleFactory.h"

#include <assert.h>

namespace tvagentapi
{

template <>
std::shared_ptr<IModule> CreateModule<IModule::Type::Chat>(
	std::weak_ptr<AgentConnection> connection)
{
	return ChatModule::Create(std::move(connection));
}

namespace
{

IChatModule::ChatEndpointType toAPIType(TVRemoteScreenSDKCommunication::ChatService::ChatType value)
{
	switch (value)
	{
		case TVRemoteScreenSDKCommunication::ChatService::ChatType::Machine:
			return IChatModule::ChatEndpointType::Machine;
		case TVRemoteScreenSDKCommunication::ChatService::ChatType::Session:
			return IChatModule::ChatEndpointType::InstantSupportSession;
	}

	assert(false);
	return IChatModule::ChatEndpointType::Machine;
}

IChatModule::ChatState toAPIType(TVRemoteScreenSDKCommunication::ChatService::ChatState value)
{
	switch (value)
	{
		case TVRemoteScreenSDKCommunication::ChatService::ChatState::Open:
			return IChatModule::ChatState::Open;
		case TVRemoteScreenSDKCommunication::ChatService::ChatState::Closed:
			return IChatModule::ChatState::Closed;
	}

	assert(false);
	return IChatModule::ChatState::Closed;
}

} // namespace

std::shared_ptr<ChatModule> ChatModule::Create(std::weak_ptr<AgentConnection> connection)
{
	std::shared_ptr<ChatModule> instance{new ChatModule(std::move(connection))};
	instance->m_weakThis = instance;
	if (!instance->registerCallbacks())
	{
		return {};
	}
	return instance;
}

void ChatModule::setCallbacks(const Callbacks& callbacks)
{
	m_callbacks = callbacks;
}

bool ChatModule::obtainChats(ObtainChatsEnumerator enumerator)
{
	if (!enumerator.callback)
	{
		return false;
	}

	auto connection = m_connection.lock();
	if (!connection)
	{
		return false;
	}

	std::vector<TVRemoteScreenSDKCommunication::ChatService::ChatInfo> chats;

	auto communicationChannel = connection->getCommunicationChannel();
	if (!communicationChannel->sendObtainChatsRequest(chats))
	{
		return false;
	}

	for (const auto& entry : chats)
	{
		if (!enumerator.callback(
			entry.chatId.c_str(),
			entry.title.c_str(),
			toAPIType(entry.chatType),
			entry.chatTypeId,
			toAPIType(entry.chatState),
			enumerator.userdata))
		{
			break;
		}
	}

	return true;
}

IChatModule::RequestID ChatModule::sendMessage(const char* chatID, const char* content)
{
	if (!chatID || !content)
	{
		return InvalidRequestID;
	}

	auto connection = m_connection.lock();
	if (!connection)
	{
		return InvalidRequestID;
	}

	auto communicationChannel = connection->getCommunicationChannel();

	if (!communicationChannel->sendSelectChatResult(chatID))
	{
		return InvalidRequestID;
	}

	if (m_nextRequestID == InvalidRequestID)
	{
		m_nextRequestID++;
	}

	const RequestID requestID = m_nextRequestID;

	if (!communicationChannel->sendMessage(requestID, content))
	{
		return InvalidRequestID;
	}

	++m_nextRequestID;

	return requestID;
}

bool ChatModule::loadMessages(const char* chatID, size_t messageCount, const char* messageBefore/* = nullptr*/)
{
	if (!chatID)
	{
		return false;
	}

	auto connection = m_connection.lock();
	if (!connection)
	{
		return false;
	}

	auto communicationChannel = connection->getCommunicationChannel();

	if (!communicationChannel->sendSelectChatResult(chatID))
	{
		return false;
	}

	return communicationChannel->loadMessages(
		static_cast<uint32_t>(messageCount),
		messageBefore ? std::string{messageBefore} : std::string{});
}

bool ChatModule::deleteHistory(const char* chatID)
{
	if (!chatID)
	{
		return false;
	}

	auto connection = m_connection.lock();
	if (!connection)
	{
		return false;
	}

	auto communicationChannel = connection->getCommunicationChannel();

	if (!communicationChannel->sendSelectChatResult(chatID))
	{
		return false;
	}

	return communicationChannel->deleteHistory();
}

bool ChatModule::deleteChat(const char* chatID)
{
	if (!chatID)
	{
		return false;
	}

	auto connection = m_connection.lock();
	if (!connection)
	{
		return false;
	}

	auto communicationChannel = connection->getCommunicationChannel();

	if (!communicationChannel->sendSelectChatResult(chatID))
	{
		return false;
	}

	return communicationChannel->deleteChat();
}

bool ChatModule::isSupported() const
{
	if (auto connection = m_connection.lock())
	{
		// TODO IOT-15139 implement IModule::isSupported()
		// return connection->isModuleSupported("TVSessionManagementModule"); // str or enum
		return true;
	}
	return false;
}

ChatModule::ChatModule(std::weak_ptr<AgentConnection> connection)
	: m_connection(std::move(connection))
{
}

bool ChatModule::registerCallbacks()
{
	auto connection = m_connection.lock();
	if (!connection)
	{
		return false;
	}

	m_connections.clear();

	auto communicationChannel = connection->getCommunicationChannel();
	auto weakDispatcher = std::weak_ptr<IDispatcher>{connection->getDispatcher()};

	const auto weakThis = m_weakThis;
	auto chatCreatedAction =
		[weakThis, weakDispatcher](
			std::string chatId,
			std::string title,
			TVRemoteScreenSDKCommunication::ChatService::ChatType sdkchatType,
			uint32_t chatTypeId)
		{
			util::weakDispatcherPost(
				weakDispatcher,
				weakThis,
				[chatId, title, sdkchatType, chatTypeId](const std::shared_ptr<ChatModule>& self)
				{
					util::safeCall(
						self->m_callbacks.chatCreated,
						chatId.c_str(),
						title.c_str(),
						toAPIType(sdkchatType),
						chatTypeId
					);
				});
		};
	m_connections.push_back(communicationChannel->chatCreated().registerCallback(chatCreatedAction));

	auto chatsRemovedAction =
		[weakThis, weakDispatcher](
			std::vector<std::string> chatIds)
		{
			util::weakDispatcherPost(
				weakDispatcher,
				weakThis,
				[chatIds](const std::shared_ptr<ChatModule>& self)
				{
					if (self->m_callbacks.chatsRemoved.callback == nullptr)
					{
						return;
					}

					std::vector<const char*> chatIdsConverted;
					chatIdsConverted.reserve(chatIds.size());
					for (const auto& entry : chatIds)
					{
						chatIdsConverted.push_back(entry.c_str());
					}

					self->m_callbacks.chatsRemoved.callback(
						chatIdsConverted.data(),
						chatIdsConverted.size(),
						self->m_callbacks.chatsRemoved.userdata
					);
				});
		};
	m_connections.push_back(communicationChannel->chatsRemoved().registerCallback(chatsRemovedAction));

	auto chatClosedAction =
		[weakThis, weakDispatcher](
			std::string chatId)
		{
			util::weakDispatcherPost(
				weakDispatcher,
				weakThis,
				[chatId](const std::shared_ptr<ChatModule>& self)
				{
					util::safeCall(
						self->m_callbacks.chatClosed,
						chatId.c_str()
					);
				});
		};
	m_connections.push_back(communicationChannel->closedChat().registerCallback(chatClosedAction));

	auto receivedMessagesAction =
		[weakThis, weakDispatcher](
			std::vector<TVRemoteScreenSDKCommunication::ChatService::ReceivedMessage> messages)
		{
			util::weakDispatcherPost(
				weakDispatcher,
				weakThis,
				[messages](const std::shared_ptr<ChatModule>& self)
				{
					if (self->m_callbacks.receivedMessages.callback == nullptr)
					{
						return;
					}

					std::vector<ChatMessage> messagesConverted;
					messagesConverted.reserve(messages.size());
					for (const auto& entry : messages)
					{
						messagesConverted.push_back(
							ChatMessage{
								entry.chatId.c_str(),
								entry.messageId.c_str(),
								entry.content.c_str(),
								entry.timeStamp,
								entry.sender.c_str()
							}
						);
					}

					self->m_callbacks.receivedMessages.callback(
						messagesConverted.data(),
						messagesConverted.size(),
						self->m_callbacks.receivedMessages.userdata
					);
				});
		};
	m_connections.push_back(communicationChannel->receivedMessages().registerCallback(receivedMessagesAction));

	auto sendMessageSuccessAction =
		[weakThis, weakDispatcher](
			uint32_t localId,
			std::string messageId,
			uint64_t timeStamp)
		{
			util::weakDispatcherPost(
				weakDispatcher,
				weakThis,
				[localId, messageId, timeStamp](const std::shared_ptr<ChatModule>& self)
				{
					util::safeCall(
						self->m_callbacks.sendMessageFinished,
						localId,
						SendMessageResult::Succeeded,
						messageId.c_str(),
						timeStamp
					);
				});
		};
	m_connections.push_back(communicationChannel->messageSent().registerCallback(sendMessageSuccessAction));

	auto sendMessageFailureAction =
		[weakThis, weakDispatcher](
			uint32_t localId)
		{
			util::weakDispatcherPost(
				weakDispatcher,
				weakThis,
				[localId](const std::shared_ptr<ChatModule>& self)
				{
					util::safeCall(
						self->m_callbacks.sendMessageFinished,
						localId,
						SendMessageResult::Failed,
						"",
						0
					);
				});
		};
	m_connections.push_back(communicationChannel->messageNotSent().registerCallback(sendMessageFailureAction));

	auto messagesLoadedAction =
		[weakThis, weakDispatcher](
			std::vector<TVRemoteScreenSDKCommunication::ChatService::ReceivedMessage> messages, bool hasMore)
		{
			util::weakDispatcherPost(
				weakDispatcher,
				weakThis,
				[messages, hasMore](const std::shared_ptr<ChatModule>& self)
				{
					std::vector<ChatMessage> messagesConverted;
					messagesConverted.reserve(messages.size());
					for (const auto& entry : messages)
					{
						messagesConverted.push_back(
							ChatMessage{
								entry.chatId.c_str(),
								entry.messageId.c_str(),
								entry.content.c_str(),
								entry.timeStamp,
								entry.sender.c_str()
							}
						);
					}
					util::safeCall(
						self->m_callbacks.messagesLoaded,
						messagesConverted.data(),
						messagesConverted.size(),
						hasMore
					);
				});
		};
	m_connections.push_back(communicationChannel->loadedMessages().registerCallback(messagesLoadedAction));

	auto historyDeletedAction =
		[weakThis, weakDispatcher](
			std::string chatId)
		{
			util::weakDispatcherPost(
				weakDispatcher,
				weakThis,
				[chatId](const std::shared_ptr<ChatModule>& self)
				{
					util::safeCall(
						self->m_callbacks.historyDeleted,
						chatId.c_str()
					);
				});
		};
	m_connections.push_back(communicationChannel->deletedHistory().registerCallback(historyDeletedAction));

	return true;
}

} // namespace tvagentapi
