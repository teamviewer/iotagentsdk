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

#include "ChatInServicegRPCServer.h"

#include <TVRemoteScreenSDKCommunication/ServiceBase/RequestMetadata.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/ServiceErrorMessage.h>

#include <grpc++/grpc++.h>

namespace TVRemoteScreenSDKCommunication
{

namespace ChatService
{

bool ChatInServicegRPCServer::StartServer(const std::string& location)
{
	m_location = location;

	m_server = TransportFW::CreateAndStartSyncServer(m_location,
		this); // Register this "service" as the instance through which we'll communicate with clients.

	return m_server != nullptr;
}

void ChatInServicegRPCServer::StopServer(bool /*force*/)
{
	if (m_server)
	{
		m_server->Shutdown();
	}
	m_server.reset();
}

ServiceType ChatInServicegRPCServer::GetServiceType() const
{
	return ServiceType::ChatIn;
}

const std::string& ChatInServicegRPCServer::GetLocation() const
{
	return m_location;
}

void ChatInServicegRPCServer::SetObtainChatsCallback(const ProcessObtainChatsRequestCallback& requestProcessing)
{
	m_obtainChatsProcessing = requestProcessing;
}

void ChatInServicegRPCServer::SetSelectChatCallback(const ProcessSelectChatRequestCallback& requestProcessing)
{
	m_selectChatProcessing = requestProcessing;
}

void ChatInServicegRPCServer::SetSendMessageCallback(const ProcessSendMessageRequestCallback& requestProcessing)
{
	m_sendMessageProcessing = requestProcessing;
}

void ChatInServicegRPCServer::SetLoadMessagesCallback(const ProcessLoadMessagesRequestCallback& requestProcessing)
{
	m_loadMessagesProcessing = requestProcessing;
}

void ChatInServicegRPCServer::SetDeleteHistoryCallback(const ProcessDeleteHistoryRequestCallback& requestProcessing)
{
	m_deleteHistoryProcessing = requestProcessing;
}

void ChatInServicegRPCServer::SetDeleteChatCallback(const ProcessDeleteChatRequestCallback& requestProcessing)
{
	m_deleteChatProcessing = requestProcessing;
}

::grpc::Status ChatInServicegRPCServer::ObtainChats(::grpc::ServerContext* context,
	const ::tvchatservice::ObtainChatsRequest* requestPtr,
	::tvchatservice::ObtainChatsResponse* responsePtr)
{
	if (context == nullptr || requestPtr == nullptr || responsePtr == nullptr)
	{
		return ::grpc::Status(::grpc::StatusCode::INTERNAL, std::string{});
	}

	if (!m_obtainChatsProcessing)
	{
		return ::grpc::Status(::grpc::StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback);
	}
	auto& request = *requestPtr;
	(void)request;

	auto& response = *responsePtr;
	(void)response;

	std::string comId;

	const auto foundComId = context->client_metadata().find(ServiceBase::CommunicationIdToken);
	if (foundComId == context->client_metadata().end())
	{
		return ::grpc::Status(::grpc::StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId);
	}
	comId = std::string((foundComId->second).data(), (foundComId->second).length());

	::grpc::Status returnStatus =
		::grpc::Status(::grpc::StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled);

	auto responseProcessing = [&returnStatus, &response](const std::vector<ChatInfo>& chatsInfos, const CallStatus& callStatus)
	{
		if (callStatus.IsOk())
		{
			returnStatus = TransportFW::Status::OK;

			for (const ChatInfo& chatInfo : chatsInfos)
			{
				::tvchatservice::ChatInfo* info = response.add_chatinfo();

				info->set_chatid(chatInfo.chatId);
				info->set_title(chatInfo.title);
				info->set_chattypeid(chatInfo.chatTypeId);

				const auto chatType = chatInfo.chatType;
				const auto chatState = chatInfo.chatState;

				::tvchatservice::ChatType chatTypeProtoValue = ::tvchatservice::ChatType::Machine;
				bool chatTypeConverted = true;
				switch (chatType)
				{
					case ChatType::Machine:
						chatTypeProtoValue = ::tvchatservice::ChatType::Machine;
						break;
					case ChatType::Session:
						chatTypeProtoValue = ::tvchatservice::ChatType::Session;
						break;
					default:
						chatTypeConverted = false;
						break;
				}

				::tvchatservice::ChatState chatStateProtoValue = ::tvchatservice::ChatState::Open;
				bool chatStateConverted = true;
				switch (chatState)
				{
					case ChatState::Open:
						chatStateProtoValue = ::tvchatservice::ChatState::Open;
						break;
					case ChatState::Closed:
						chatStateProtoValue = ::tvchatservice::ChatState::Closed;
						break;
					default:
						chatStateConverted = false;
						break;
				}

				if (!chatTypeConverted || !chatStateConverted)
				{
					returnStatus = TransportFW::Status{TransportFW::StatusCode::CANCELLED,
						TvServiceBase::ErrorMessage_UnexpectedEnumValue};
				}
				info->set_chattype(chatTypeProtoValue);
				info->set_chatstate(chatStateProtoValue);
			}
		}
		else
		{
			returnStatus = TransportFW::Status{TransportFW::StatusCode::ABORTED, callStatus.errorMessage};
		}
	};

	m_obtainChatsProcessing(comId, responseProcessing);

	return returnStatus;
}

::grpc::Status ChatInServicegRPCServer::SelectChat(::grpc::ServerContext* context,
	const ::tvchatservice::SelectChatRequest* requestPtr,
	::tvchatservice::SelectChatResponse* responsePtr)
{
	if (context == nullptr || requestPtr == nullptr || responsePtr == nullptr)
	{
		return ::grpc::Status(::grpc::StatusCode::INTERNAL, std::string{});
	}

	if (!m_selectChatProcessing)
	{
		return ::grpc::Status(::grpc::StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback);
	}
	auto& request = *requestPtr;
	(void)request;

	auto& response = *responsePtr;
	(void)response;

	std::string comId;

	const auto foundComId = context->client_metadata().find(ServiceBase::CommunicationIdToken);
	if (foundComId == context->client_metadata().end())
	{
		return ::grpc::Status(::grpc::StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId);
	}
	comId = std::string((foundComId->second).data(), (foundComId->second).length());

	::grpc::Status returnStatus =
		::grpc::Status(::grpc::StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled);

	auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
	{
		if (callStatus.IsOk())
		{
			returnStatus = ::grpc::Status::OK;
		}
		else
		{
			returnStatus = ::grpc::Status(::grpc::StatusCode::ABORTED, callStatus.errorMessage);
		}
	};

	m_selectChatProcessing(comId,
		request.chatid(),

		responseProcessing);

	return returnStatus;
}

::grpc::Status ChatInServicegRPCServer::SendMessage(::grpc::ServerContext* context,
	const ::tvchatservice::SendMessageRequest* requestPtr,
	::tvchatservice::SendMessageResponse* responsePtr)
{
	if (context == nullptr || requestPtr == nullptr || responsePtr == nullptr)
	{
		return ::grpc::Status(::grpc::StatusCode::INTERNAL, std::string{});
	}

	if (!m_sendMessageProcessing)
	{
		return ::grpc::Status(::grpc::StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback);
	}
	auto& request = *requestPtr;
	(void)request;

	auto& response = *responsePtr;
	(void)response;

	std::string comId;

	const auto foundComId = context->client_metadata().find(ServiceBase::CommunicationIdToken);
	if (foundComId == context->client_metadata().end())
	{
		return ::grpc::Status(::grpc::StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId);
	}
	comId = std::string((foundComId->second).data(), (foundComId->second).length());

	::grpc::Status returnStatus =
		::grpc::Status(::grpc::StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled);

	auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
	{
		if (callStatus.IsOk())
		{
			returnStatus = ::grpc::Status::OK;
		}
		else
		{
			returnStatus = ::grpc::Status(::grpc::StatusCode::ABORTED, callStatus.errorMessage);
		}
	};

	m_sendMessageProcessing(comId,
		request.localid(),

		request.content(),

		responseProcessing);

	return returnStatus;
}

::grpc::Status ChatInServicegRPCServer::LoadMessages(::grpc::ServerContext* context,
	const ::tvchatservice::LoadMessagesRequest* requestPtr,
	::tvchatservice::LoadMessagesResponse* responsePtr)
{
	if (context == nullptr || requestPtr == nullptr || responsePtr == nullptr)
	{
		return ::grpc::Status(::grpc::StatusCode::INTERNAL, std::string{});
	}

	if (!m_loadMessagesProcessing)
	{
		return ::grpc::Status(::grpc::StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback);
	}
	auto& request = *requestPtr;
	(void)request;

	auto& response = *responsePtr;
	(void)response;

	std::string comId;

	const auto foundComId = context->client_metadata().find(ServiceBase::CommunicationIdToken);
	if (foundComId == context->client_metadata().end())
	{
		return ::grpc::Status(::grpc::StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId);
	}
	comId = std::string((foundComId->second).data(), (foundComId->second).length());

	::grpc::Status returnStatus =
		::grpc::Status(::grpc::StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled);

	auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
	{
		if (callStatus.IsOk())
		{
			returnStatus = ::grpc::Status::OK;
		}
		else
		{
			returnStatus = ::grpc::Status(::grpc::StatusCode::ABORTED, callStatus.errorMessage);
		}
	};

	m_loadMessagesProcessing(comId,
		request.count(),

		request.lastid(),

		responseProcessing);

	return returnStatus;
}

::grpc::Status ChatInServicegRPCServer::DeleteHistory(::grpc::ServerContext* context,
	const ::tvchatservice::DeleteHistoryRequest* requestPtr,
	::tvchatservice::DeleteHistoryResponse* responsePtr)
{
	if (context == nullptr || requestPtr == nullptr || responsePtr == nullptr)
	{
		return ::grpc::Status(::grpc::StatusCode::INTERNAL, std::string{});
	}

	if (!m_deleteHistoryProcessing)
	{
		return ::grpc::Status(::grpc::StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback);
	}
	auto& request = *requestPtr;
	(void)request;

	auto& response = *responsePtr;
	(void)response;

	std::string comId;

	const auto foundComId = context->client_metadata().find(ServiceBase::CommunicationIdToken);
	if (foundComId == context->client_metadata().end())
	{
		return ::grpc::Status(::grpc::StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId);
	}
	comId = std::string((foundComId->second).data(), (foundComId->second).length());

	::grpc::Status returnStatus =
		::grpc::Status(::grpc::StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled);

	auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
	{
		if (callStatus.IsOk())
		{
			returnStatus = ::grpc::Status::OK;
		}
		else
		{
			returnStatus = ::grpc::Status(::grpc::StatusCode::ABORTED, callStatus.errorMessage);
		}
	};

	m_deleteHistoryProcessing(comId, responseProcessing);

	return returnStatus;
}

::grpc::Status ChatInServicegRPCServer::DeleteChat(::grpc::ServerContext* context,
	const ::tvchatservice::DeleteChatRequest* requestPtr,
	::tvchatservice::DeleteChatResponse* responsePtr)
{
	if (context == nullptr || requestPtr == nullptr || responsePtr == nullptr)
	{
		return ::grpc::Status(::grpc::StatusCode::INTERNAL, std::string{});
	}

	if (!m_deleteChatProcessing)
	{
		return ::grpc::Status(::grpc::StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback);
	}
	auto& request = *requestPtr;
	(void)request;

	auto& response = *responsePtr;
	(void)response;

	std::string comId;

	const auto foundComId = context->client_metadata().find(ServiceBase::CommunicationIdToken);
	if (foundComId == context->client_metadata().end())
	{
		return ::grpc::Status(::grpc::StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId);
	}
	comId = std::string((foundComId->second).data(), (foundComId->second).length());

	::grpc::Status returnStatus =
		::grpc::Status(::grpc::StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled);

	auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
	{
		if (callStatus.IsOk())
		{
			returnStatus = ::grpc::Status::OK;
		}
		else
		{
			returnStatus = ::grpc::Status(::grpc::StatusCode::ABORTED, callStatus.errorMessage);
		}
	};

	m_deleteChatProcessing(comId, responseProcessing);

	return returnStatus;
}

} // namespace ChatService

} // namespace TVRemoteScreenSDKCommunication