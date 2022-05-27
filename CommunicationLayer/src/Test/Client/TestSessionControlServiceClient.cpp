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
#include "TestSessionControlServiceClient.h"

#include "TestData/TestDataSessionControl.h"

#include <TVRemoteScreenSDKCommunication/SessionControlService/ISessionControlServiceClient.h>
#include <TVRemoteScreenSDKCommunication/SessionControlService/ServiceFactory.h>

#include <iostream>

namespace TestSessionControlService
{

constexpr const char* LogPrefix = "[SessionControlService][Client] ";

int TestSessionControlServiceClient(int /*argc*/, char** /*argv*/)
{
	using namespace TVRemoteScreenSDKCommunication::SessionControlService;

	const std::shared_ptr<ISessionControlServiceClient> client = ServiceFactory::CreateClient();

	if (client->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::SessionControl)
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

	TVRemoteScreenSDKCommunication::CallStatus response = client->ChangeControlMode(TestData::ComId, TestData::Mode);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "ChangeControlMode successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "ChangeControlMode Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	response = client->CloseRc(TestData::ComId);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "CloseRc successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "CloseRc Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	ISessionControlServiceClient::TVSessionsResult tvsessionsResponse
		= client->GetRunningTVSessions(TestData::ComId);

	std::vector<int32_t> testSessions{TestData::RunningTVSessions};
	if (tvsessionsResponse.IsOk() && tvsessionsResponse.tvSessionIDs == testSessions)
	{
		std::cout << LogPrefix << "GetRunningTVSessions successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "GetRunningTVSessions Error: " << response.errorMessage << std::endl;
		std::cerr << "received sessionIDs: {";
		for (const auto entry : tvsessionsResponse.tvSessionIDs)
		{
			std::cerr << ' ' << entry;
		}
		std::cerr << " }" <<std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
} // namespace TestSessionControlService
