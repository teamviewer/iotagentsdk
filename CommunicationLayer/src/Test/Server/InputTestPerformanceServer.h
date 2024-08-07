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

#include <TVRemoteScreenSDKCommunication/InputService/IInputServiceServer.h>
#include <TVRemoteScreenSDKCommunication/InputService/ServiceFactory.h>

#include <atomic>
#include <iostream>
#include <memory>
#include <string>

namespace TestInputServicePerformance
{

struct SharedInformation
{
	std::atomic<uint64_t> counter{0};
};

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
std::shared_ptr<TVRemoteScreenSDKCommunication::InputService::IInputServiceServer> TestInputService(
	SharedInformation& information,
	const std::string& location)
{
	using namespace TVRemoteScreenSDKCommunication::InputService;
	const std::shared_ptr<IInputServiceServer> server = ServiceFactory::CreateServer<Framework>();

	const auto receiveFunction = [&information](
		const std::string& /*communicationId*/,
		TVRemoteScreenSDKCommunication::InputService::KeyState /*keyState*/,
		uint32_t /*xkbSymbol*/,
		uint32_t /*unicodeCharacter*/,
		uint32_t /*xkbModifiers*/,
		const TVRemoteScreenSDKCommunication::InputService::IInputServiceServer::SimulateKeyResponseCallback& response)
	{
		++information.counter;
		response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
	};

	server->SetSimulateKeyCallback(receiveFunction);

	if (!server->StartServer(location))
	{
		std::cerr << "Error: Failed to start server at location '" << location
			<< "'. Please check that location URL is correct and contains a scheme\n";
		return {};
	}

	return server;
}

} // namespace TestInputServicePerformance
