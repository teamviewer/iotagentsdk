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

#include "ChatInService.grpc.pb.h"

#include <TVRemoteScreenSDKCommunication/ChatService/IChatInServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/AbstractServiceServer.h>

namespace TVRemoteScreenSDKCommunication
{
namespace ChatService
{

struct CallbacksMeta
{
	enum class Enum : size_t
	{
		ObtainChats,
		SelectChat,
		SendMessage,
		LoadMessages,
		DeleteHistory,
		DeleteChat,

		EnumSize
	};

	using Callbacks = std::tuple<
		IChatInServiceServer::ProcessObtainChatsRequestCallback,
		IChatInServiceServer::ProcessSelectChatRequestCallback,
		IChatInServiceServer::ProcessSendMessageRequestCallback,
		IChatInServiceServer::ProcessLoadMessagesRequestCallback,
		IChatInServiceServer::ProcessDeleteHistoryRequestCallback,
		IChatInServiceServer::ProcessDeleteChatRequestCallback>;
};

class ChatInServiceServer :
	public ServiceBase::AbstractServiceServer<
		IChatInServiceServer,
		::tvchatservice::ChatInService,
		CallbacksMeta>
{
public:
	ChatInServiceServer();
	~ChatInServiceServer() override;

	void SetProcessObtainChatsCallback(
		const ProcessObtainChatsRequestCallback& requestProcessing) override;

	::grpc::Status ObtainChats(
		::grpc::ServerContext* context,
		const ::tvchatservice::ObtainChatsRequest* request,
		::tvchatservice::ObtainChatsResponse* response) override;

	void SetProcessSelectChatCallback(
		const ProcessSelectChatRequestCallback& requestProcessing) override;

	::grpc::Status SelectChat(
		::grpc::ServerContext* context,
		const ::tvchatservice::SelectChatRequest* request,
		::tvchatservice::SelectChatResponse* response) override;

	void SetProcessSendMessageCallback(
		const ProcessSendMessageRequestCallback& requestProcessing) override;

	::grpc::Status SendMessage(
		::grpc::ServerContext* context,
		const ::tvchatservice::SendMessageRequest* request,
		::tvchatservice::SendMessageResponse* response) override;

	void SetProcessLoadMessagesCallback(
		const ProcessLoadMessagesRequestCallback& requestProcessing) override;

	::grpc::Status LoadMessages(
		::grpc::ServerContext* context,
		const ::tvchatservice::LoadMessagesRequest* request,
		::tvchatservice::LoadMessagesResponse* response) override;

	void SetProcessDeleteHistoryCallback(
		const ProcessDeleteHistoryRequestCallback& requestProcessing) override;

	::grpc::Status DeleteHistory(
		::grpc::ServerContext* context,
		const ::tvchatservice::DeleteHistoryRequest* request,
		::tvchatservice::DeleteHistoryResponse* response) override;

	void SetProcessDeleteChatCallback(
		const ProcessDeleteChatRequestCallback& requestProcessing) override;

	::grpc::Status DeleteChat(
		::grpc::ServerContext* context,
		const ::tvchatservice::DeleteChatRequest* request,
		::tvchatservice::DeleteChatResponse* response) override;
};

} // namespace ChatService
} // namespace TVRemoteScreenSDKCommunication
