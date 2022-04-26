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
#include "TestInputServiceClient.h"

#include "TestData/TestDataInput.h"

#include <TVRemoteScreenSDKCommunication/InputService/IInputServiceClient.h>
#include <TVRemoteScreenSDKCommunication/InputService/ServiceFactory.h>

#include <memory>
#include <iostream>
#include <string>

namespace TestInputService
{

constexpr const char* LogPrefix = "[InputService][Client] ";

int TestInputServiceClient(int /*argc*/, char** /*argv*/)
{
	using namespace TVRemoteScreenSDKCommunication::InputService;
	const std::shared_ptr<IInputServiceClient> client = ServiceFactory::CreateClient();
	if (client->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::Input)
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

	TVRemoteScreenSDKCommunication::CallStatus response = TVRemoteScreenSDKCommunication::CallState::Failed;

	response = client->SimulateKey(TestData::ComId, TestData::KeyState, TestData::XkbSymbol, TestData::UnicodeCharacter, TestData::XkbModifiers);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "SimulateKey successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "SimulateKey Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	response = client->SimulateMouseMove(TestData::ComId, TestData::PosX, TestData::PosY);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "SimulateMouseMove successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "SimulateMouseMove Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	response = client->SimulateMousePressRelease(TestData::ComId, TestData::KeyState, TestData::PosX, TestData::PosY, TestData::Button);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "SimulateMousePressRelease successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "SimulateMousePressRelease Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	response = client->SimulateMouseWheel(TestData::ComId, TestData::PosX, TestData::PosY, TestData::Angle);
	if (response.IsOk())
	{
		std::cout << LogPrefix << "SimulateMouseWheel successful " << std::endl;
	}
	else
	{
		std::cerr << LogPrefix << "SimulateMouseWheel Error: " << response.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

} // namespace TestInputService
