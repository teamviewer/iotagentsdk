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
#include "ChatOutServiceServer.h"

#include <vector>

namespace TVRemoteScreenSDKCommunication
{
namespace ChatService
{

ChatOutServiceServer::ChatOutServiceServer()
	: BaseType{ServiceType::ChatOut}
{
}

ChatOutServiceServer::~ChatOutServiceServer() = default;

void ChatOutServiceServer::SetProcessChatCreatedRequestCallback(
	const ProcessChatCreatedRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::ChatCreated>(requestProcessing);
}

void ChatOutServiceServer::SetProcessChatsRemovedRequestCallback(
	const ProcessChatsRemovedRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::ChatsRemoved>(requestProcessing);
}

void ChatOutServiceServer::SetProcessReceivedMessagesRequestCallback(
	const ProcessReceivedMessagesRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::ReceivedMessages>(requestProcessing);
}

void ChatOutServiceServer::SetProcessMessageSentRequestCallback(
	const ProcessMessageSentRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::MessageSent>(requestProcessing);
}

void ChatOutServiceServer::SetProcessMessageNotSentRequestCallback(
	const ProcessMessageNotSentRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::MessageNotSent>(requestProcessing);
}

void ChatOutServiceServer::SetProcessLoadedMessagesRequestCallback(
	const ProcessLoadedMessagesRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::LoadedMessages>(requestProcessing);
}

void ChatOutServiceServer::SetProcessDeletedHistoryRequestCallback(
	const ProcessDeletedHistoryRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::DeletedHistory>(requestProcessing);
}

void ChatOutServiceServer::SetProcessClosedChatRequestCallback(
	const ProcessClosedChatRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::ClosedChat>(requestProcessing);
}

::grpc::Status ChatOutServiceServer::ChatCreated(
	::grpc::ServerContext* context,
	const ::tvchatservice::ChatCreatedRequest* request,
	::tvchatservice::ChatCreatedResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::ChatCreated,
		::tvchatservice::ChatCreatedRequest>
	{
		static ::grpc::Status ValidateRequest(const Request& request)
		{
			switch (request.chattype())
			{
				case ::tvchatservice::ChatType::Machine:
				case ::tvchatservice::ChatType::Session:
					break;
				default:
					return ::grpc::Status{
						::grpc::StatusCode::CANCELLED,
						TvServiceBase::ErrorMessage_UnexpectedEnumValue};
			}
			return ::grpc::Status::OK;
		}

		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			ChatType chatType = ChatType::Machine;

			switch (request.chattype())
			{
				case ::tvchatservice::ChatType::Machine:
					chatType = ChatType::Machine;
					break;
				case ::tvchatservice::ChatType::Session:
					chatType = ChatType::Session;
					break;
				default:;
			}

			callback(
				comId,
				request.chatid(),
				request.title(),
				chatType,
				request.chattypeid(),
				responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status ChatOutServiceServer::ChatsRemoved(
	::grpc::ServerContext* context,
	const ::tvchatservice::ChatsRemovedRequest* request,
	::tvchatservice::ChatsRemovedResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::ChatsRemoved,
		::tvchatservice::ChatsRemovedRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			std::vector<std::string> chatIds(request.chatid_size());

			for (int index = 0; index < request.chatid_size(); ++index)
			{
				chatIds[index] = request.chatid(index);
			}

			callback(comId, chatIds, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status ChatOutServiceServer::ReceivedMessages(
	::grpc::ServerContext* context,
	const ::tvchatservice::ReceivedMessagesRequest* request,
	::tvchatservice::ReceivedMessagesResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::ReceivedMessages,
		::tvchatservice::ReceivedMessagesRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			std::vector<ReceivedMessage> messages(request.received_size());

			for (int index = 0; index < request.received_size(); ++index)
			{
				const ::tvchatservice::ReceivedMessage& received =
					request.received(index);

				messages[index].chatId = received.chatid();
				messages[index].messageId = received.messageid();
				messages[index].content = received.content();
				messages[index].timeStamp = received.timestamp();
				messages[index].sender = received.sender();
			}

			callback(comId, messages, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status ChatOutServiceServer::MessageSent(
	::grpc::ServerContext* context,
	const ::tvchatservice::MessageSentRequest* request,
	::tvchatservice::MessageSentResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::MessageSent,
		::tvchatservice::MessageSentRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			callback(
				comId,
				request.localid(),
				request.messageid(),
				request.timestamp(),
				responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status ChatOutServiceServer::MessageNotSent(
	::grpc::ServerContext* context,
	const ::tvchatservice::MessageNotSentRequest* request,
	::tvchatservice::MessageNotSentResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::MessageNotSent,
		::tvchatservice::MessageNotSentRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			callback(comId, request.localid(), responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status ChatOutServiceServer::LoadedMessages(
	::grpc::ServerContext* context,
	const ::tvchatservice::LoadedMessagesRequest* request,
	::tvchatservice::LoadedMessagesResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::LoadedMessages,
		::tvchatservice::LoadedMessagesRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			std::vector<ReceivedMessage> messages(request.messages_size());

			for (int index = 0; index < request.messages_size(); ++index)
			{
				const ::tvchatservice::ReceivedMessage& rMessages =
					request.messages(index);

				messages[index].chatId = rMessages.chatid();
				messages[index].messageId = rMessages.messageid();
				messages[index].content = rMessages.content();
				messages[index].timeStamp = rMessages.timestamp();
				messages[index].sender = rMessages.sender();
			}

			callback(comId, messages, request.hasmore(), responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status ChatOutServiceServer::DeletedHistory(
	::grpc::ServerContext* context,
	const ::tvchatservice::DeletedHistoryRequest* request,
	::tvchatservice::DeletedHistoryResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::DeletedHistory,
		::tvchatservice::DeletedHistoryRequest>
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

::grpc::Status ChatOutServiceServer::ClosedChat(
	::grpc::ServerContext* context,
	const ::tvchatservice::ClosedChatRequest* request,
	::tvchatservice::ClosedChatResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::ClosedChat,
		::tvchatservice::ClosedChatRequest>
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

} // namespace ChatOutService
} // namespace TVRemoteScreenSDKCommunication
