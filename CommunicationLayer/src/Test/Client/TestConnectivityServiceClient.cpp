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
#include "TestConnectivityServiceClient.h"

#include "TestData/TestDataConnectivity.h"

#include <TVRemoteScreenSDKCommunication/ConnectivityService/IConnectivityServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ConnectivityService/ServiceFactory.h>

#include <iostream>

namespace TestConnectivityService
{

constexpr const char* LogPrefix = "[ConnectivityService][Client] ";

int TestConnectivityServiceClient(int /*argc*/, char** /*argv*/)
{
	using namespace TVRemoteScreenSDKCommunication::ConnectivityService;

	const std::shared_ptr<IConnectivityServiceClient> client = ServiceFactory::CreateClient();
	if (client->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::Connectivity)
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

	response = client->IsAvailable(TestData::ComId);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "IsAvailable successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "IsAvailable Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	response = client->Disconnect(TestData::ComId);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "Disconnect successful" << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "Disconnect Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

} // namespace TestConnectivityService
