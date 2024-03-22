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
// ==================================
// Generated by TVCMGen. DO NOT EDIT!
// ==================================

#include "ChatOutServiceSocketIOServer.h"

#include <TVRemoteScreenSDKCommunication/ServiceBase/RequestMetadata.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/ServiceErrorMessage.h>

#include "internal/ChatOutServiceOutFunctions.h"

#include "Chat.pb.h"
#include "ChatOutMessages.pb.h"

namespace TVRemoteScreenSDKCommunication
{

namespace ChatService
{

bool ChatOutServiceSocketIOServer::StartServer(const std::string& location)
{
	using namespace Transport::SocketIO;

	m_location = location;
	Server::ServerFunctionMap functions;
	{
		auto* requestProcessing = &m_ChatCreatedProcessing;
		functions[Function_ChatCreated] = [requestProcessing](const std::string& comIdValue,
											  std::shared_ptr<std::string> requestRaw,
											  std::shared_ptr<std::string> responseRaw)
		{
			if (!*requestProcessing)
			{
				return Status{StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback};
			}
			::tvchatservice::ChatCreatedRequest request;
			if (!request.ParseFromString(*requestRaw))
			{
				return Status(StatusCode::IO_ERROR, "error parsing request");
			}

			requestRaw->clear();

			::tvchatservice::ChatCreatedResponse response;

			Status status = [&]()
			{
				std::string comId = comIdValue;

				if (comIdValue.empty())
				{
					return Status{StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId};
				}

				Status returnStatus{StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled};

				auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
				{
					if (callStatus.IsOk())
					{
						returnStatus = Status::OK;
					}
					else
					{
						returnStatus = Status{StatusCode::ABORTED, callStatus.errorMessage};
					}
				};

				auto& m_chatCreatedProcessing = *requestProcessing;

				const auto chatType = request.chattype();

				ChatType chatTypeEnumValue = ChatType::Machine;
				bool chatTypeConverted = true;
				switch (chatType)
				{
					case ::tvchatservice::ChatType::Machine:
						chatTypeEnumValue = ChatType::Machine;
						break;
					case ::tvchatservice::ChatType::Session:
						chatTypeEnumValue = ChatType::Session;
						break;
					default:
						chatTypeConverted = false;
						break;
				}

				if (!chatTypeConverted)
				{
					return TransportFW::Status{TransportFW::StatusCode::CANCELLED, TvServiceBase::ErrorMessage_UnexpectedEnumValue};
				}

				m_chatCreatedProcessing(comId,
					request.chatid(),

					request.title(),

					chatTypeEnumValue,

					request.chattypeid(),

					responseProcessing);

				return returnStatus;
			}();
			if (!status.ok())
			{
				return status;
			}

			if (!response.SerializeToString(responseRaw.get()))
			{
				return Status(StatusCode::IO_ERROR, "response serialization failed");
			}

			return status;
		};
	}

	{
		auto* requestProcessing = &m_ChatsRemovedProcessing;
		functions[Function_ChatsRemoved] = [requestProcessing](const std::string& comIdValue,
											   std::shared_ptr<std::string> requestRaw,
											   std::shared_ptr<std::string> responseRaw)
		{
			if (!*requestProcessing)
			{
				return Status{StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback};
			}
			::tvchatservice::ChatsRemovedRequest request;
			if (!request.ParseFromString(*requestRaw))
			{
				return Status(StatusCode::IO_ERROR, "error parsing request");
			}

			requestRaw->clear();

			::tvchatservice::ChatsRemovedResponse response;

			Status status = [&]()
			{
				std::string comId = comIdValue;

				if (comIdValue.empty())
				{
					return Status{StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId};
				}

				Status returnStatus{StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled};

				auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
				{
					if (callStatus.IsOk())
					{
						returnStatus = Status::OK;
					}
					else
					{
						returnStatus = Status{StatusCode::ABORTED, callStatus.errorMessage};
					}
				};

				auto& m_chatsRemovedProcessing = *requestProcessing;

				std::vector<std::string> chatIdRepeated(request.chatid_size());
				for (int index = 0; index < request.chatid_size(); ++index)
				{
					chatIdRepeated[index] = request.chatid(index);
				}

				m_chatsRemovedProcessing(comId,
					chatIdRepeated,

					responseProcessing);

				return returnStatus;
			}();
			if (!status.ok())
			{
				return status;
			}

			if (!response.SerializeToString(responseRaw.get()))
			{
				return Status(StatusCode::IO_ERROR, "response serialization failed");
			}

			return status;
		};
	}

	{
		auto* requestProcessing = &m_ReceivedMessagesProcessing;
		functions[Function_ReceivedMessages] = [requestProcessing](const std::string& comIdValue,
												   std::shared_ptr<std::string> requestRaw,
												   std::shared_ptr<std::string> responseRaw)
		{
			if (!*requestProcessing)
			{
				return Status{StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback};
			}
			::tvchatservice::ReceivedMessagesRequest request;
			if (!request.ParseFromString(*requestRaw))
			{
				return Status(StatusCode::IO_ERROR, "error parsing request");
			}

			requestRaw->clear();

			::tvchatservice::ReceivedMessagesResponse response;

			Status status = [&]()
			{
				std::string comId = comIdValue;

				if (comIdValue.empty())
				{
					return Status{StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId};
				}

				Status returnStatus{StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled};

				auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
				{
					if (callStatus.IsOk())
					{
						returnStatus = Status::OK;
					}
					else
					{
						returnStatus = Status{StatusCode::ABORTED, callStatus.errorMessage};
					}
				};

				auto& m_receivedMessagesProcessing = *requestProcessing;

				std::vector<::tvchatservice::ReceivedMessage> receivedRepeated(request.received_size());
				for (int index = 0; index < request.received_size(); ++index)
				{
					receivedRepeated[index] = request.received(index);
				}

				std::vector<ReceivedMessage> messages(request.received_size());

				for (int index = 0; index < request.received_size(); ++index)
				{
					const ::tvchatservice::ReceivedMessage& received = request.received(index);

					messages[index].chatId = received.chatid();
					messages[index].messageId = received.messageid();
					messages[index].content = received.content();
					messages[index].timeStamp = received.timestamp();
					messages[index].sender = received.sender();
				}

				m_receivedMessagesProcessing(comId, messages, responseProcessing);

				return returnStatus;
			}();
			if (!status.ok())
			{
				return status;
			}

			if (!response.SerializeToString(responseRaw.get()))
			{
				return Status(StatusCode::IO_ERROR, "response serialization failed");
			}

			return status;
		};
	}

	{
		auto* requestProcessing = &m_MessageSentProcessing;
		functions[Function_MessageSent] = [requestProcessing](const std::string& comIdValue,
											  std::shared_ptr<std::string> requestRaw,
											  std::shared_ptr<std::string> responseRaw)
		{
			if (!*requestProcessing)
			{
				return Status{StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback};
			}
			::tvchatservice::MessageSentRequest request;
			if (!request.ParseFromString(*requestRaw))
			{
				return Status(StatusCode::IO_ERROR, "error parsing request");
			}

			requestRaw->clear();

			::tvchatservice::MessageSentResponse response;

			Status status = [&]()
			{
				std::string comId = comIdValue;

				if (comIdValue.empty())
				{
					return Status{StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId};
				}

				Status returnStatus{StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled};

				auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
				{
					if (callStatus.IsOk())
					{
						returnStatus = Status::OK;
					}
					else
					{
						returnStatus = Status{StatusCode::ABORTED, callStatus.errorMessage};
					}
				};

				auto& m_messageSentProcessing = *requestProcessing;

				m_messageSentProcessing(comId,
					request.localid(),

					request.messageid(),

					request.timestamp(),

					responseProcessing);

				return returnStatus;
			}();
			if (!status.ok())
			{
				return status;
			}

			if (!response.SerializeToString(responseRaw.get()))
			{
				return Status(StatusCode::IO_ERROR, "response serialization failed");
			}

			return status;
		};
	}

	{
		auto* requestProcessing = &m_MessageNotSentProcessing;
		functions[Function_MessageNotSent] = [requestProcessing](const std::string& comIdValue,
												 std::shared_ptr<std::string> requestRaw,
												 std::shared_ptr<std::string> responseRaw)
		{
			if (!*requestProcessing)
			{
				return Status{StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback};
			}
			::tvchatservice::MessageNotSentRequest request;
			if (!request.ParseFromString(*requestRaw))
			{
				return Status(StatusCode::IO_ERROR, "error parsing request");
			}

			requestRaw->clear();

			::tvchatservice::MessageNotSentResponse response;

			Status status = [&]()
			{
				std::string comId = comIdValue;

				if (comIdValue.empty())
				{
					return Status{StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId};
				}

				Status returnStatus{StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled};

				auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
				{
					if (callStatus.IsOk())
					{
						returnStatus = Status::OK;
					}
					else
					{
						returnStatus = Status{StatusCode::ABORTED, callStatus.errorMessage};
					}
				};

				auto& m_messageNotSentProcessing = *requestProcessing;

				m_messageNotSentProcessing(comId,
					request.localid(),

					responseProcessing);

				return returnStatus;
			}();
			if (!status.ok())
			{
				return status;
			}

			if (!response.SerializeToString(responseRaw.get()))
			{
				return Status(StatusCode::IO_ERROR, "response serialization failed");
			}

			return status;
		};
	}

	{
		auto* requestProcessing = &m_LoadedMessagesProcessing;
		functions[Function_LoadedMessages] = [requestProcessing](const std::string& comIdValue,
												 std::shared_ptr<std::string> requestRaw,
												 std::shared_ptr<std::string> responseRaw)
		{
			if (!*requestProcessing)
			{
				return Status{StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback};
			}
			::tvchatservice::LoadedMessagesRequest request;
			if (!request.ParseFromString(*requestRaw))
			{
				return Status(StatusCode::IO_ERROR, "error parsing request");
			}

			requestRaw->clear();

			::tvchatservice::LoadedMessagesResponse response;

			Status status = [&]()
			{
				std::string comId = comIdValue;

				if (comIdValue.empty())
				{
					return Status{StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId};
				}

				Status returnStatus{StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled};

				auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
				{
					if (callStatus.IsOk())
					{
						returnStatus = Status::OK;
					}
					else
					{
						returnStatus = Status{StatusCode::ABORTED, callStatus.errorMessage};
					}
				};

				auto& m_loadedMessagesProcessing = *requestProcessing;

				std::vector<::tvchatservice::ReceivedMessage> messagesRepeated(request.messages_size());
				for (int index = 0; index < request.messages_size(); ++index)
				{
					messagesRepeated[index] = request.messages(index);
				}

				std::vector<ReceivedMessage> messages(request.messages_size());

				for (int index = 0; index < request.messages_size(); ++index)
				{
					const ::tvchatservice::ReceivedMessage& rMessages = request.messages(index);

					messages[index].chatId = rMessages.chatid();
					messages[index].messageId = rMessages.messageid();
					messages[index].content = rMessages.content();
					messages[index].timeStamp = rMessages.timestamp();
					messages[index].sender = rMessages.sender();
				}

				m_loadedMessagesProcessing(comId, messages, request.hasmore(), responseProcessing);

				return returnStatus;
			}();
			if (!status.ok())
			{
				return status;
			}

			if (!response.SerializeToString(responseRaw.get()))
			{
				return Status(StatusCode::IO_ERROR, "response serialization failed");
			}

			return status;
		};
	}

	{
		auto* requestProcessing = &m_DeletedHistoryProcessing;
		functions[Function_DeletedHistory] = [requestProcessing](const std::string& comIdValue,
												 std::shared_ptr<std::string> requestRaw,
												 std::shared_ptr<std::string> responseRaw)
		{
			if (!*requestProcessing)
			{
				return Status{StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback};
			}
			::tvchatservice::DeletedHistoryRequest request;
			if (!request.ParseFromString(*requestRaw))
			{
				return Status(StatusCode::IO_ERROR, "error parsing request");
			}

			requestRaw->clear();

			::tvchatservice::DeletedHistoryResponse response;

			Status status = [&]()
			{
				std::string comId = comIdValue;

				if (comIdValue.empty())
				{
					return Status{StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId};
				}

				Status returnStatus{StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled};

				auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
				{
					if (callStatus.IsOk())
					{
						returnStatus = Status::OK;
					}
					else
					{
						returnStatus = Status{StatusCode::ABORTED, callStatus.errorMessage};
					}
				};

				auto& m_deletedHistoryProcessing = *requestProcessing;

				m_deletedHistoryProcessing(comId,
					request.chatid(),

					responseProcessing);

				return returnStatus;
			}();
			if (!status.ok())
			{
				return status;
			}

			if (!response.SerializeToString(responseRaw.get()))
			{
				return Status(StatusCode::IO_ERROR, "response serialization failed");
			}

			return status;
		};
	}

	{
		auto* requestProcessing = &m_ClosedChatProcessing;
		functions[Function_ClosedChat] = [requestProcessing](const std::string& comIdValue,
											 std::shared_ptr<std::string> requestRaw,
											 std::shared_ptr<std::string> responseRaw)
		{
			if (!*requestProcessing)
			{
				return Status{StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback};
			}
			::tvchatservice::ClosedChatRequest request;
			if (!request.ParseFromString(*requestRaw))
			{
				return Status(StatusCode::IO_ERROR, "error parsing request");
			}

			requestRaw->clear();

			::tvchatservice::ClosedChatResponse response;

			Status status = [&]()
			{
				std::string comId = comIdValue;

				if (comIdValue.empty())
				{
					return Status{StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId};
				}

				Status returnStatus{StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled};

				auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
				{
					if (callStatus.IsOk())
					{
						returnStatus = Status::OK;
					}
					else
					{
						returnStatus = Status{StatusCode::ABORTED, callStatus.errorMessage};
					}
				};

				auto& m_closedChatProcessing = *requestProcessing;

				m_closedChatProcessing(comId,
					request.chatid(),

					responseProcessing);

				return returnStatus;
			}();
			if (!status.ok())
			{
				return status;
			}

			if (!response.SerializeToString(responseRaw.get()))
			{
				return Status(StatusCode::IO_ERROR, "response serialization failed");
			}

			return status;
		};
	}

	m_server.reset(new Server(std::move(functions)));

	return m_server->Start(location);
}

void ChatOutServiceSocketIOServer::StopServer(bool /*force*/)
{
	if (m_server)
	{
		m_server->Shutdown();
	}
	m_server.reset();
}

ServiceType ChatOutServiceSocketIOServer::GetServiceType() const
{
	return ServiceType::ChatOut;
}

const std::string& ChatOutServiceSocketIOServer::GetLocation() const
{
	return m_location;
}

void ChatOutServiceSocketIOServer::SetChatCreatedCallback(const ProcessChatCreatedRequestCallback& requestProcessing)
{
	m_ChatCreatedProcessing = requestProcessing;
}

void ChatOutServiceSocketIOServer::SetChatsRemovedCallback(const ProcessChatsRemovedRequestCallback& requestProcessing)
{
	m_ChatsRemovedProcessing = requestProcessing;
}

void ChatOutServiceSocketIOServer::SetReceivedMessagesCallback(const ProcessReceivedMessagesRequestCallback& requestProcessing)
{
	m_ReceivedMessagesProcessing = requestProcessing;
}

void ChatOutServiceSocketIOServer::SetMessageSentCallback(const ProcessMessageSentRequestCallback& requestProcessing)
{
	m_MessageSentProcessing = requestProcessing;
}

void ChatOutServiceSocketIOServer::SetMessageNotSentCallback(const ProcessMessageNotSentRequestCallback& requestProcessing)
{
	m_MessageNotSentProcessing = requestProcessing;
}

void ChatOutServiceSocketIOServer::SetLoadedMessagesCallback(const ProcessLoadedMessagesRequestCallback& requestProcessing)
{
	m_LoadedMessagesProcessing = requestProcessing;
}

void ChatOutServiceSocketIOServer::SetDeletedHistoryCallback(const ProcessDeletedHistoryRequestCallback& requestProcessing)
{
	m_DeletedHistoryProcessing = requestProcessing;
}

void ChatOutServiceSocketIOServer::SetClosedChatCallback(const ProcessClosedChatRequestCallback& requestProcessing)
{
	m_ClosedChatProcessing = requestProcessing;
}

} // namespace ChatService

} // namespace TVRemoteScreenSDKCommunication