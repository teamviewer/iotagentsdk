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
#include "ChatInServiceClient.h"

#include <TVRemoteScreenSDKCommunication/ServiceBase/ClientErrorMessage.h>

namespace TVRemoteScreenSDKCommunication
{
namespace ChatService
{

ChatInServiceClient::ChatInServiceClient()
	: BaseType{ServiceType::ChatIn}
{
}

IChatInServiceClient::ObtainChatsResponse ChatInServiceClient::ObtainChats(
	const std::string& comId)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvchatservice::ObtainChatsRequest;
		using Response = ::tvchatservice::ObtainChatsResponse;
		using Return = ObtainChatsResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response)
		{
			return stub.ObtainChats(&context, request, &response);
		}

		static Return HandleResponse(
			const ::grpc::Status& status,
			Response& response)
		{
			ObtainChatsResponse returnValue;

			if (status.ok())
			{
				returnValue.state = CallState::Ok;

				std::vector<ChatInfo> chats(response.chatinfo_size());

				for (int index = 0; index < response.chatinfo_size(); ++index)
				{
					const ::tvchatservice::ChatInfo& info = response.chatinfo(index);

					chats[index].chatId = info.chatid();
					chats[index].title = info.title();

					switch (info.chattype())
					{
						case tvchatservice::ChatType::Machine:
							chats[index].chatType = ChatType::Machine;
							break;
						case tvchatservice::ChatType::Session:
							chats[index].chatType = ChatType::Session;
							break;
						default:
							returnValue.errorMessage =
								TvServiceBase::ErrorMessage_InvalidResponseValue;
							return returnValue;
					}

					chats[index].chatTypeId = info.chattypeid();

					switch (info.chatstate())
					{
						case tvchatservice::ChatState::Open:
							chats[index].chatState = ChatState::Open;
							break;
						case tvchatservice::ChatState::Closed:
							chats[index].chatState = ChatState::Closed;
							break;
						default:
							returnValue.errorMessage =
								TvServiceBase::ErrorMessage_InvalidResponseValue;
							return returnValue;
					}
				}

				returnValue.chats.swap(chats);
			}
			else
			{
				returnValue.errorMessage = status.error_message();
			}

			return returnValue;
		}
	};

	return ClientCall<Meta>(comId);
}

CallStatus ChatInServiceClient::SelectChat(
	const std::string& comId,
	std::string chatId)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvchatservice::SelectChatRequest;
		using Response = ::tvchatservice::SelectChatResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			const std::string& chatId)
		{
			request.set_chatid(chatId);

			return stub.SelectChat(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, chatId);
}

CallStatus ChatInServiceClient::SendMessage(
	const std::string& comId,
	uint32_t localId,
	std::string content)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvchatservice::SendMessageRequest;
		using Response = ::tvchatservice::SendMessageResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			uint32_t localId,
			const std::string& content)
		{
			request.set_localid(localId);
			request.set_content(content);

			return stub.SendMessage(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, localId, content);
}

CallStatus ChatInServiceClient::LoadMessages(
	const std::string& comId,
	uint32_t count,
	std::string lastId)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvchatservice::LoadMessagesRequest;
		using Response = ::tvchatservice::LoadMessagesResponse;

		static bool ValidateInput(const std::string& comId, uint32_t count, ...)
		{
			return DefaultMeta::ValidateInput(comId) && count != 0;
		}

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			uint32_t count,
			const std::string& lastId)
		{
			request.set_count(count);
			request.set_lastid(lastId);

			return stub.LoadMessages(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, count, lastId);
}

CallStatus ChatInServiceClient::DeleteHistory(const std::string& comId)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvchatservice::DeleteHistoryRequest;
		using Response = ::tvchatservice::DeleteHistoryResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response)
		{
			return stub.DeleteHistory(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId);
}

CallStatus ChatInServiceClient::DeleteChat(const std::string& comId)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvchatservice::DeleteChatRequest;
		using Response = ::tvchatservice::DeleteChatResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response)
		{
			return stub.DeleteChat(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId);
}

} // namespace ChatService
} // namespace TVRemoteScreenSDKCommunication
