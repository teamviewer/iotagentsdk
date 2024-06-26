//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2024 TeamViewer Germany GmbH                                     //
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
// ==================================
// Generated by TVCMGen. DO NOT EDIT!
// ==================================
#pragma once

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/CallStatus.h>
#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/IServiceClient.h>

#include <TVRemoteScreenSDKCommunication/InputService/MouseButton.h>

#include <TVRemoteScreenSDKCommunication/InputService/KeyState.h>

#include <cstdint>
#include <string>
#include <vector>

namespace TVRemoteScreenSDKCommunication
{

namespace InputService
{

class IInputServiceClient : public IServiceClient
{
public:
	virtual ~IInputServiceClient() = default;

	// rpc call SimulateKey
	virtual CallStatus SimulateKey(const std::string& comId,
		KeyState keystate,
		uint32_t xkbSymbol,
		uint32_t unicodeCharacter,
		uint32_t xkbModifiers) = 0;

	// rpc call SimulateMouseMove
	virtual CallStatus SimulateMouseMove(const std::string& comId, int32_t posX, int32_t posY) = 0;

	// rpc call SimulateMousePressRelease
	virtual CallStatus SimulateMousePressRelease(const std::string& comId,
		MouseButtonState mouseButtonState,
		int32_t posX,
		int32_t posY,
		MouseButton button) = 0;

	// rpc call SimulateMouseWheel
	virtual CallStatus SimulateMouseWheel(const std::string& comId, int32_t posX, int32_t posY, int32_t angle) = 0;
};

} // namespace InputService

} // namespace TVRemoteScreenSDKCommunication