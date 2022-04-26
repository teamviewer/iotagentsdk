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

#include "InputService.grpc.pb.h"

#include <TVRemoteScreenSDKCommunication/InputService/IInputServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/AbstractServiceClient.h>

namespace TVRemoteScreenSDKCommunication
{
namespace InputService
{

class InputServiceClient :
	public ServiceBase::AbstractServiceClient<
		IInputServiceClient,
		::tvinputservice::InputService>
{
public:
	InputServiceClient();
	~InputServiceClient() override = default;

	CallStatus SimulateKey(
		const std::string& comId,
		KeyState keyState,
		uint32_t xkbSymbol,
		uint32_t unicodeCharacter,
		uint32_t xkbModifiers) override;

	CallStatus SimulateMouseMove(
		const std::string& comId,
		int32_t posX,
		int32_t posY) override;

	CallStatus SimulateMousePressRelease(
		const std::string& comId,
		KeyState keyState,
		int32_t posX,
		int32_t posY,
		MouseButton button) override;

	CallStatus SimulateMouseWheel(
		const std::string& comId,
		int32_t posX,
		int32_t posY,
		int32_t angle) override;
};

} // namespace InputService
} // namespace TVRemoteScreenSDKCommunication
