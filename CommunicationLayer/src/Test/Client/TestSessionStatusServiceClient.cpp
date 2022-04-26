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
#include "TestSessionStatusServiceClient.h"

#include "TestData/TestDataSessionStatus.h"

#include <TVRemoteScreenSDKCommunication/SessionStatusService/ISessionStatusServiceClient.h>
#include <TVRemoteScreenSDKCommunication/SessionStatusService/ServiceFactory.h>

#include <iostream>

namespace TestSessionStatusService
{

constexpr const char* LogPrefix = "[SessionStatusService][Client] ";

int TestSessionStatusServiceClient(int /*argc*/, char** /*argv*/)
{
	using namespace TVRemoteScreenSDKCommunication::SessionStatusService;
	const std::shared_ptr<ISessionStatusServiceClient> client = ServiceFactory::CreateClient();
	if (client->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::SessionStatus)
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

	TVRemoteScreenSDKCommunication::CallStatus response;

	response = client->RemoteControlStarted(TestData::ComId, TestData::GrabStrategy);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "StartedRemoteControl successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "StartedRemoteControl Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	response = client->RemoteControlStopped(TestData::ComId);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "StoppedRemoteControl successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "StoppedRemoteControl Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	response = client->TVSessionStarted(TestData::ComId, TestData::DummyTVSessionID, TestData::SessionCountStarted);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "TVSessionStarted successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "TVSessionStarted Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	response = client->TVSessionStopped(TestData::ComId, TestData::DummyTVSessionID, TestData::SessionCountStopped);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "TVSessionStopped successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "TVSessionStopped Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

} // namespace TestSessionStatusService
