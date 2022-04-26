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

#include "ChatOutService.grpc.pb.h"

#include <TVRemoteScreenSDKCommunication/ChatService/IChatOutServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/AbstractServiceServer.h>

namespace TVRemoteScreenSDKCommunication
{
namespace ChatService
{

struct CallbacksMeta
{
	enum class Enum : size_t
	{
		ChatCreated,
		ChatsRemoved,
		ReceivedMessages,
		MessageSent,
		MessageNotSent,
		LoadedMessages,
		DeletedHistory,
		ClosedChat,

		EnumSize
	};

	using Callbacks = std::tuple<
		IChatOutServiceServer::ProcessChatCreatedRequestCallback,
		IChatOutServiceServer::ProcessChatsRemovedRequestCallback,
		IChatOutServiceServer::ProcessReceivedMessagesRequestCallback,
		IChatOutServiceServer::ProcessMessageSentRequestCallback,
		IChatOutServiceServer::ProcessMessageNotSentRequestCallback,
		IChatOutServiceServer::ProcessLoadedMessagesRequestCallback,
		IChatOutServiceServer::ProcessDeletedHistoryRequestCallback,
		IChatOutServiceServer::ProcessClosedChatRequestCallback>;
};

class ChatOutServiceServer :
	public ServiceBase::AbstractServiceServer<
		IChatOutServiceServer,
		::tvchatservice::ChatOutService,
		CallbacksMeta>
{
public:
	ChatOutServiceServer();
	~ChatOutServiceServer() override;

	void SetProcessChatCreatedRequestCallback(
		const ProcessChatCreatedRequestCallback& requestProcessing) override;

	::grpc::Status ChatCreated(
		::grpc::ServerContext* context,
		const ::tvchatservice::ChatCreatedRequest* request,
		::tvchatservice::ChatCreatedResponse* response) override;

	void SetProcessChatsRemovedRequestCallback(
		const ProcessChatsRemovedRequestCallback& requestProcessing) override;

	::grpc::Status ChatsRemoved(
		::grpc::ServerContext* context,
		const ::tvchatservice::ChatsRemovedRequest* request,
		::tvchatservice::ChatsRemovedResponse* response) override;

	void SetProcessReceivedMessagesRequestCallback(
		const ProcessReceivedMessagesRequestCallback& requestProcessing) override;

	::grpc::Status ReceivedMessages(
		::grpc::ServerContext* context,
		const ::tvchatservice::ReceivedMessagesRequest* request,
		::tvchatservice::ReceivedMessagesResponse* response) override;

	void SetProcessMessageSentRequestCallback(
		const ProcessMessageSentRequestCallback& requestProcessing) override;

	::grpc::Status MessageSent(
		::grpc::ServerContext* context,
		const ::tvchatservice::MessageSentRequest* request,
		::tvchatservice::MessageSentResponse* response) override;

	void SetProcessMessageNotSentRequestCallback(
		const ProcessMessageNotSentRequestCallback& requestProcessing) override;

	::grpc::Status MessageNotSent(
		::grpc::ServerContext* context,
		const ::tvchatservice::MessageNotSentRequest* request,
		::tvchatservice::MessageNotSentResponse* response) override;

	void SetProcessLoadedMessagesRequestCallback(
		const ProcessLoadedMessagesRequestCallback& requestProcessing) override;

	::grpc::Status LoadedMessages(
		::grpc::ServerContext* context,
		const ::tvchatservice::LoadedMessagesRequest* request,
		::tvchatservice::LoadedMessagesResponse* response) override;

	void SetProcessDeletedHistoryRequestCallback(
		const ProcessDeletedHistoryRequestCallback& requestProcessing) override;

	::grpc::Status DeletedHistory(
		::grpc::ServerContext* context,
		const ::tvchatservice::DeletedHistoryRequest* request,
		::tvchatservice::DeletedHistoryResponse* response) override;

	void SetProcessClosedChatRequestCallback(
		const ProcessClosedChatRequestCallback& requestProcessing) override;

	::grpc::Status ClosedChat(
		::grpc::ServerContext* context,
		const ::tvchatservice::ClosedChatRequest* request,
		::tvchatservice::ClosedChatResponse* response) override;
};

} // namespace ChatService
} // namespace TVRemoteScreenSDKCommunication
