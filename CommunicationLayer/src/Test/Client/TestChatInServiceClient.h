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

#include "TestData/TestDataChatIn.h"

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/TransportFramework.h>

#include <TVRemoteScreenSDKCommunication/ChatService/IChatInServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ChatService/InServiceFactory.h>

#include <iostream>

namespace TestChatInService
{

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
int TestChatInServiceClient(int /*argc*/, char** /*argv*/)
{
	using namespace TVRemoteScreenSDKCommunication::ChatService;
	using TestData = TestData<Framework>;
	const std::string LogPrefix = "[ChatInService][Client][fw=" + std::to_string(Framework) + "] ";

	const std::shared_ptr<IChatInServiceClient> client = InServiceFactory::CreateClient<Framework>();

	if (client->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::ChatIn)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	client->StartClient(TestData::Socket);
	if (client->GetDestination() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	IChatInServiceClient::ObtainChatsResponse obtainChatsResponse = client->ObtainChats(TestData::ComId);

	if (obtainChatsResponse.IsOk() &&
		obtainChatsResponse.chats.size() == 1 &&
		obtainChatsResponse.chats[0].chatId == TestData::ChatId &&
		obtainChatsResponse.chats[0].title == TestData::ChatTitle &&
		obtainChatsResponse.chats[0].chatType == TestData::ChatType &&
		obtainChatsResponse.chats[0].chatTypeId == TestData::ChatTypeId &&
		obtainChatsResponse.chats[0].chatState == TestData::ChatState)
	{
		std::cout << LogPrefix << "ObtainChats successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "ObtainChats Error: " << obtainChatsResponse.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	TVRemoteScreenSDKCommunication::CallStatus selectChatResponse = client->SelectChat(TestData::ComId, TestData::ChatId);

	if (selectChatResponse.IsOk())
	{
		std::cout << LogPrefix << "SelectChat successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "SelectChat Error: " << selectChatResponse.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	TVRemoteScreenSDKCommunication::CallStatus sendMessageResponse = client->SendMessage(TestData::ComId, TestData::LocalId, TestData::Content);
	if (sendMessageResponse.IsOk())
	{
		std::cout << LogPrefix << "sendMessageResponse successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "sendMessageResponse Error: " << sendMessageResponse.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	TVRemoteScreenSDKCommunication::CallStatus loadMessagesResponse = client->LoadMessages(TestData::ComId, TestData::Count, TestData::MessageId);
	if (loadMessagesResponse.IsOk())
	{
		std::cout << LogPrefix << "loadMessagesResponse successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "loadMessagesResponse Error: " << loadMessagesResponse.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	TVRemoteScreenSDKCommunication::CallStatus deleteHistoryResponse = client->DeleteHistory(TestData::ComId);
	if (deleteHistoryResponse.IsOk())
	{
		std::cout << LogPrefix << "DeleteHistoryResponse successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "DeleteHistoryResponse Error: " << deleteHistoryResponse.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	TVRemoteScreenSDKCommunication::CallStatus deleteChatResponse = client->DeleteChat(TestData::ComId);
	if (deleteChatResponse.IsOk())
	{
		std::cout << LogPrefix << "DeleteChatResponse successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "DeleteChatResponse Error: " << deleteChatResponse.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

} // namespace TestChatInService
