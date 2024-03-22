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

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/TransportFramework.h>

#include <TVRemoteScreenSDKCommunication/InputService/IInputServiceClient.h>
#include <TVRemoteScreenSDKCommunication/InputService/ServiceFactory.h>

#include <atomic>
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>

namespace TestInputServicePerformance
{

constexpr const char* LogPrefix = "[InputService][Client] ";
constexpr const char* ComId = "token";
constexpr TVRemoteScreenSDKCommunication::InputService::KeyState KeyState = TVRemoteScreenSDKCommunication::InputService::KeyState::Down;
constexpr uint32_t XkbSymbol = 14;
constexpr uint32_t UnicodeCharacter = 16;
constexpr uint32_t XkbModifiers = 123456;

struct StopCondition
{
	std::atomic_bool run{false};
};

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
int TestInputServiceClient(StopCondition& stopCondition, const std::string& location)
{
	using namespace TVRemoteScreenSDKCommunication::InputService;
	const std::shared_ptr<IInputServiceClient> client = ServiceFactory::CreateClient<Framework>();

	client->StartClient(location);

	TVRemoteScreenSDKCommunication::CallStatus response{};

	int errorCounter = 0;

	while (stopCondition.run)
	{
		response = client->SimulateKey(ComId, KeyState, XkbSymbol, UnicodeCharacter, XkbModifiers);
		if (response.IsOk() == false)
		{
			std::cerr << "Response failed because: " << response.errorMessage << std::endl;
			if (errorCounter > 10)
			{
				return EXIT_FAILURE;
			}
			++errorCounter;
			sleep(1);
		}
		else
		{
			errorCounter = 0;
		}
	}
	return EXIT_SUCCESS;
}

} // namespace TestInputServicePerformance
