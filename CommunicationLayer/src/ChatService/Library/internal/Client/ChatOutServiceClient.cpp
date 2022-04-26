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
#include "ChatOutServiceClient.h"

#include <TVRemoteScreenSDKCommunication/ServiceBase/ClientErrorMessage.h>

namespace TVRemoteScreenSDKCommunication
{
namespace ChatService
{

ChatOutServiceClient::ChatOutServiceClient()
	: BaseType{ServiceType::ChatOut}
{
}

CallStatus ChatOutServiceClient::ChatCreated(
	const std::string& comId,
	std::string chatId,
	std::string title,
	ChatType chatType,
	uint32_t chatTypeId)
{
	::tvchatservice::ChatType type = ::tvchatservice::ChatType::Machine;
	switch (chatType)
	{
		case ChatType::Machine:
			type = ::tvchatservice::ChatType::Machine;
			break;
		case ChatType::Session:
			type = ::tvchatservice::ChatType::Session;
			break;
		default:
			return CallStatus{
				CallState::Failed,
				TvServiceBase::ErrorMessage_InvalidInputParameter};
	}

	struct Meta: DefaultMeta
	{
		using Request = ::tvchatservice::ChatCreatedRequest;
		using Response = ::tvchatservice::ChatCreatedResponse;

		static bool ValidateInput(const std::string& comId, const std::string& chatId, ...)
		{
			return DefaultMeta::ValidateInput(comId) && !chatId.empty();
		}

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			const std::string& chatId,
			const std::string& title,
			::tvchatservice::ChatType chatType,
			uint32_t chatTypeId)
		{
			request.set_chatid(chatId);
			request.set_title(title);
			request.set_chattype(chatType);
			request.set_chattypeid(chatTypeId);

			return stub.ChatCreated(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, chatId, title, type, chatTypeId);
}

CallStatus ChatOutServiceClient::ChatsRemoved(
	const std::string& comId,
	std::vector<std::string> chatIds)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvchatservice::ChatsRemovedRequest;
		using Response = ::tvchatservice::ChatsRemovedResponse;

		static bool ValidateInput(
			const std::string& comId,
			const std::vector<std::string>& chatIds)
		{
			return DefaultMeta::ValidateInput(comId) &&
				std::all_of(
					chatIds.begin(),
					chatIds.end(),
					[](const std::string& chatId)
					{
						return !chatId.empty();
					});
		}

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			const std::vector<std::string>& chatIds)
		{
			for (const auto& chatid : chatIds)
			{
				request.add_chatid(chatid);
			}

			return stub.ChatsRemoved(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, chatIds);
}

CallStatus ChatOutServiceClient::ReceivedMessages(
	const std::string& comId,
	std::vector<ReceivedMessage> messages)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvchatservice::ReceivedMessagesRequest;
		using Response = ::tvchatservice::ReceivedMessagesResponse;

		static bool ValidateInput(
			const std::string& comId,
			const std::vector<ReceivedMessage>& messages)
		{
			return DefaultMeta::ValidateInput(comId) &&
				std::all_of(
					messages.begin(),
					messages.end(),
					[](const ReceivedMessage& message)
					{
						return !message.messageId.empty() &&
							!message.content.empty() &&
							message.timeStamp != 0;
					});
		}

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			const std::vector<ReceivedMessage>& messages)
		{
			for (const auto& message : messages)
			{
				::tvchatservice::ReceivedMessage* receivedMessage =
					request.add_received();
				receivedMessage->set_chatid(message.chatId);
				receivedMessage->set_messageid(message.messageId);
				receivedMessage->set_content(message.content);
				receivedMessage->set_timestamp(message.timeStamp);
				receivedMessage->set_sender(message.sender);
			}

			return stub.ReceivedMessages(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, messages);
}

CallStatus ChatOutServiceClient::MessageSent(
	const std::string& comId,
	uint32_t localId,
	std::string messageId,
	uint64_t timeStamp)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvchatservice::MessageSentRequest;
		using Response = ::tvchatservice::MessageSentResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			uint32_t localId,
			std::string messageId,
			uint64_t timeStamp)
		{
			request.set_localid(localId);
			request.set_messageid(messageId);
			request.set_timestamp(timeStamp);

			return stub.MessageSent(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, localId, messageId, timeStamp);
}

CallStatus ChatOutServiceClient::MessageNotSent(
	const std::string& comId,
	uint32_t localId)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvchatservice::MessageNotSentRequest;
		using Response = ::tvchatservice::MessageNotSentResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			uint32_t localId)
		{
			request.set_localid(localId);

			return stub.MessageNotSent(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, localId);
}

CallStatus ChatOutServiceClient::LoadedMessages(
	const std::string& comId,
	std::vector<ReceivedMessage> messages,
	bool hasMore)
{
	for (const auto& message : messages)
	{
		if (message.messageId.empty() ||
			message.content.empty() ||
			message.timeStamp == 0)
		{
			return CallStatus{
				CallState::Failed,
				TvServiceBase::ErrorMessage_InvalidInputParameter};
		}
	}

	struct Meta: DefaultMeta
	{
		using Request = ::tvchatservice::LoadedMessagesRequest;
		using Response = ::tvchatservice::LoadedMessagesResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			const std::vector<ReceivedMessage>& messages,
			bool hasMore)
		{
			for (const auto& message : messages)
			{
				::tvchatservice::ReceivedMessage* rMessage = request.add_messages();
				rMessage->set_chatid(message.chatId);
				rMessage->set_messageid(message.messageId);
				rMessage->set_content(message.content);
				rMessage->set_timestamp(message.timeStamp);
				rMessage->set_sender(message.sender);
			}
			request.set_hasmore(hasMore);

			return stub.LoadedMessages(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, messages, hasMore);
}

CallStatus ChatOutServiceClient::DeletedHistory(
	const std::string& comId,
	std::string chatId)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvchatservice::DeletedHistoryRequest;
		using Response = ::tvchatservice::DeletedHistoryResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			const std::string& chatId)
		{
			request.set_chatid(chatId);

			return stub.DeletedHistory(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, chatId);
}

CallStatus ChatOutServiceClient::ClosedChat(
	const std::string& comId,
	std::string chatId)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvchatservice::ClosedChatRequest;
		using Response = ::tvchatservice::ClosedChatResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			const std::string& chatId)
		{
			request.set_chatid(chatId);

			return stub.ClosedChat(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, chatId);
}

} // namespace ChatService
} // namespace TVRemoteScreenSDKCommunication
