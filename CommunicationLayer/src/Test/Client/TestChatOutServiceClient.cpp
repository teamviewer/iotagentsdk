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
#include "TestChatOutServiceClient.h"

#include "TestData/TestDataChatOut.h"

#include <TVRemoteScreenSDKCommunication/ChatService/IChatOutServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ChatService/OutServiceFactory.h>

#include <memory>
#include <iostream>
#include <string>

namespace TestChatOutService
{

constexpr const char* LogPrefix = "[ChatOutService][Client] ";

int TestChatOutServiceClient(int /*argc*/, char** /*argv*/)
{
	using namespace TVRemoteScreenSDKCommunication::ChatService;

	TVRemoteScreenSDKCommunication::CallStatus response;
	const std::shared_ptr<IChatOutServiceClient> client = OutServiceFactory::CreateClient();

	if (client->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::ChatOut)
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

	response = client->ChatCreated(TestData::ComId, TestData::ChatId, TestData::ChatTitle, TestData::ChatType, TestData::ChatTypeId);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "ChatCreated successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "ChatCreated Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	std::vector<std::string> chatIds{TestData::ChatId};
	response = client->ChatsRemoved(TestData::ComId, chatIds);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "ChatRemoved successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "ChatRemoved Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	std::vector<TVRemoteScreenSDKCommunication::ChatService::ReceivedMessage> receivedMessages{
		TVRemoteScreenSDKCommunication::ChatService::ReceivedMessage{
			TestData::ChatId, TestData::MessageId, TestData::Content, TestData::TimeStamp, TestData::Sender}};

	response = client->ReceivedMessages(TestData::ComId, receivedMessages);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "ReceivedMessage successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "ReceivedMessage Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	response = client->MessageSent(TestData::ComId, TestData::LocalId, TestData::MessageId, TestData::TimeStamp);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "MessageSent successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "MessageSent Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	response = client->MessageNotSent(TestData::ComId, TestData::LocalId);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "MessageNotSent successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "MessageNotSent Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	response = client->LoadedMessages(TestData::ComId, receivedMessages, false);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "LoadedMessages successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "LoadedMessages Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	response = client->DeletedHistory(TestData::ComId, TestData::ChatId);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "DeletedHistory successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "DeletedHistory Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	response = client->ClosedChat(TestData::ComId, TestData::ChatId);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "ClosedChat successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "ClosedChat Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

} // namespace TestChatOutService
