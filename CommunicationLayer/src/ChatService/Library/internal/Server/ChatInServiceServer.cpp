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
#include "ChatInServiceServer.h"

namespace TVRemoteScreenSDKCommunication
{
namespace ChatService
{

ChatInServiceServer::ChatInServiceServer()
	: BaseType{ServiceType::ChatIn}
{
}

ChatInServiceServer::~ChatInServiceServer() = default;

void ChatInServiceServer::SetProcessObtainChatsCallback(
	const ProcessObtainChatsRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::ObtainChats>(requestProcessing);
}

void ChatInServiceServer::SetProcessSelectChatCallback(
	const ProcessSelectChatRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::SelectChat>(requestProcessing);
}

void ChatInServiceServer::SetProcessSendMessageCallback(
	const ProcessSendMessageRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::SendMessage>(requestProcessing);
}

void ChatInServiceServer::SetProcessLoadMessagesCallback(
	const ProcessLoadMessagesRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::LoadMessages>(requestProcessing);
}

void ChatInServiceServer::SetProcessDeleteHistoryCallback(
	const ProcessDeleteHistoryRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::DeleteHistory>(requestProcessing);
}

void ChatInServiceServer::SetProcessDeleteChatCallback(
	const ProcessDeleteChatRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::DeleteChat>(requestProcessing);
}

::grpc::Status ChatInServiceServer::ObtainChats(
	::grpc::ServerContext* context,
	const ::tvchatservice::ObtainChatsRequest* request,
	::tvchatservice::ObtainChatsResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::ObtainChats,
		::tvchatservice::ObtainChatsRequest>
	{
		using ResponseProcessing = std::function<void(
			const std::vector<ChatInfo>& chatsInfos,
			const CallStatus& callStatus)>;

		static ResponseProcessing MakeResponseProcessing(
			::grpc::Status& returnStatus,
			::tvchatservice::ObtainChatsResponse& response)
		{
			return [&returnStatus, &response](
				const std::vector<ChatInfo>& chatsInfos,
				const CallStatus& callStatus)
			{
				if (callStatus.IsOk())
				{
					returnStatus = ::grpc::Status::OK;

					for (const ChatInfo& chatInfo : chatsInfos)
					{
						::tvchatservice::ChatInfo* info = response.add_chatinfo();

						info->set_chatid(chatInfo.chatId);
						info->set_title(chatInfo.title);

						switch (chatInfo.chatType)
						{
							case ChatType::Machine:
								info->set_chattype(tvchatservice::ChatType::Machine);
								break;
							case ChatType::Session:
								info->set_chattype(tvchatservice::ChatType::Session);
								break;
							default:
								returnStatus = ::grpc::Status{
									::grpc::StatusCode::CANCELLED,
									TvServiceBase::ErrorMessage_UnexpectedEnumValue};
								break;
						}

						info->set_chattypeid(chatInfo.chatTypeId);

						switch (chatInfo.chatState)
						{
							case ChatState::Open:
								info->set_chatstate(tvchatservice::ChatState::Open);
								break;
							case ChatState::Closed:
								info->set_chatstate(tvchatservice::ChatState::Closed);
								break;
							default:
								returnStatus = ::grpc::Status{
									::grpc::StatusCode::CANCELLED,
									TvServiceBase::ErrorMessage_UnexpectedEnumValue};
								break;
						}
					}
				}
				else
				{
					returnStatus = ::grpc::Status{
						::grpc::StatusCode::ABORTED,
						callStatus.errorMessage};
				}
			};
		}

		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& /*request*/,
			const ResponseProcessing& responseProcessing)
		{
			callback(comId, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status ChatInServiceServer::SelectChat(
	::grpc::ServerContext* context,
	const ::tvchatservice::SelectChatRequest* request,
	::tvchatservice::SelectChatResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::SelectChat,
		::tvchatservice::SelectChatRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			callback(comId, request.chatid(), responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status ChatInServiceServer::SendMessage(
	::grpc::ServerContext* context,
	const ::tvchatservice::SendMessageRequest* request,
	::tvchatservice::SendMessageResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::SendMessage,
		::tvchatservice::SendMessageRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			callback(comId, request.localid(), request.content(), responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status ChatInServiceServer::LoadMessages(
	::grpc::ServerContext* context,
	const ::tvchatservice::LoadMessagesRequest* request,
	::tvchatservice::LoadMessagesResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::LoadMessages,
		::tvchatservice::LoadMessagesRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			callback(comId, request.count(), request.lastid(), responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status ChatInServiceServer::DeleteHistory(
	::grpc::ServerContext* context,
	const ::tvchatservice::DeleteHistoryRequest* request,
	::tvchatservice::DeleteHistoryResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::DeleteHistory,
		::tvchatservice::DeleteHistoryRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& /*request*/,
			const ResponseProcessing& responseProcessing)
		{
			callback(comId, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status ChatInServiceServer::DeleteChat(
	::grpc::ServerContext* context,
	const ::tvchatservice::DeleteChatRequest* request,
	::tvchatservice::DeleteChatResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::DeleteChat,
		::tvchatservice::DeleteChatRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& /*request*/,
			const ResponseProcessing& responseProcessing)
		{
			callback(comId, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

} // namespace ChatService
} // namespace TVRemoteScreenSDKCommunication
