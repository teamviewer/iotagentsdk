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
#include "TestInputService.h"

#include "TestData/TestDataInput.h"

#include <TVRemoteScreenSDKCommunication/InputService/ServiceFactory.h>

#include <string>
#include <memory>
#include <stdio.h>
#include <iostream>
#include <thread>

namespace TestInputService
{

constexpr const char* LogPrefix = "[InputService][Server] ";

std::shared_ptr<TVRemoteScreenSDKCommunication::InputService::IInputServiceServer> TestInputService()
{
	using namespace TVRemoteScreenSDKCommunication::InputService;
	const std::shared_ptr<IInputServiceServer> server = ServiceFactory::CreateServer();
	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::Input)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	const auto simulateKey = [](
		const std::string& comId,
		KeyState keyState,
		uint32_t xkbSymbol,
		uint32_t unicodeCharacter,
		uint32_t xkbModifiers,
		const IInputServiceServer::KeyResponseCallback& response)
	{
		std::cout << LogPrefix << "Received simulate key with: " << comId << "(comId), ";
		if (keyState == KeyState::Down)
		{
			std::cout << "KeyState::Down, ";
		}
		else
		{
			std::cout << "KeyState::Up, ";
		}

		std::cout
			<< xkbSymbol
			<< "(xkbSymbol), "
			<< unicodeCharacter
			<< "(unicode), "
			<< xkbModifiers
			<< "(mod)"
			<< std::endl;

		if (comId == TestData::ComId && keyState == TestData::KeyState && xkbSymbol == TestData::XkbSymbol && unicodeCharacter == TestData::UnicodeCharacter && xkbModifiers == TestData::XkbModifiers)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetReceivedKeyCallback(simulateKey);

	const auto simulateMouseMove = [](
		const std::string& comId,
		int32_t posX,
		int32_t posY,
		const IInputServiceServer::MouseMoveResponseCallback& response)
	{
		std::cout
			<< LogPrefix
			<< "Received simulate mouse move with: "
			<< comId
			<< "(comId), "
			<< posX
			<< "(x), "
			<< posY
			<< "(y), "
			<< std::endl;

		if (comId == TestData::ComId && posX == TestData::PosX && posY == TestData::PosY)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetMouseMoveCallback(simulateMouseMove);

	const auto simulateMousePressRelease = [](
		const std::string& comId,
		KeyState keyState,
		int32_t posX,
		int32_t posY,
		MouseButton button,
		const IInputServiceServer::MousePressReleaseResponseCallback& response)
	{
		std::cout << LogPrefix << "Received simulate mouse press release with: " << comId << "(comId), ";
		if (keyState == KeyState::Down)
		{
			std::cout << "KeyState::Down, ";
		}
		else
		{
			std::cout << "KeyState::Up, ";
		}
		std::cout << posX << "(x), " << posY << "(y), ";
		switch (button)
		{
			case MouseButton::Left:
				std::cout << " MouseButton::Left" << std::endl;
				break;
			case MouseButton::Middle:
				std::cout << " MouseButton::Middle" << std::endl;
				break;
			case MouseButton::Right:
				std::cout << " MouseButton::Right" << std::endl;
				break;
			case MouseButton::Unknown:
				std::cout << " MouseButton::Unknown" << std::endl;
				break;
		}

		if (comId == TestData::ComId && keyState == TestData::KeyState && posX == TestData::PosX && posY == TestData::PosY && button == TestData::Button)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetMousePressReleaseCallback(simulateMousePressRelease);

	const auto simulateMouseWheel = [](
		const std::string& comId,
		int32_t posX,
		int32_t posY,
		int32_t angle,
		const IInputServiceServer::MouseWheelResponseCallback& response)
	{
		std::cout
			<< LogPrefix
			<< "Received simulate mouse wheel with: "
			<< comId
			<< "(comId), "
			<< posX
			<< "(x), "
			<< posY
			<< "(y), "
			<< angle
			<< "(angle)"
			<< std::endl;

		if (comId == TestData::ComId && posX == TestData::PosX && posY == TestData::PosY && angle == TestData::Angle)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetMouseWheelCallback(simulateMouseWheel);

	server->StartServer(TestData::Socket);
	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}

} // namespace TestInputService
