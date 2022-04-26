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

#include "MouseButton.h"
#include "KeyState.h"

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/CallStatus.h>
#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/IServiceServer.h>

#include <string>
#include <functional>
#include <vector>

namespace TVRemoteScreenSDKCommunication
{
namespace InputService
{

class IInputServiceServer : public IServiceServer
{
public:
	virtual ~IInputServiceServer() = default;

	using KeyResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessKeyRequestCallback = std::function<void(
		const std::string& appToken,
		KeyState keyState,
		uint32_t xkbSymbol,
		uint32_t unicodeCharacter,
		uint32_t xkbModifiers,
		const KeyResponseCallback& response)>;

	virtual void SetReceivedKeyCallback(const ProcessKeyRequestCallback& requestProcessing) = 0;

	using MouseMoveResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessMouseMoveRequestCallback = std::function<void(
		const std::string& comId,
		int32_t posX,
		int32_t posY,
		const MouseMoveResponseCallback& response)>;

	virtual void SetMouseMoveCallback(const ProcessMouseMoveRequestCallback& requestProcessing) = 0;

	using MousePressReleaseResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessMousePressReleaseRequestCallback = std::function<void(
		const std::string& comId,
		KeyState keyState,
		int32_t posX,
		int32_t posY,
		MouseButton button,
		const MousePressReleaseResponseCallback& response)>;

	virtual void SetMousePressReleaseCallback(const ProcessMousePressReleaseRequestCallback& requestProcessing) = 0;

	using MouseWheelResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessMouseWheelRequestCallback = std::function<void(
		const std::string& comId,
		int32_t posX,
		int32_t posY,
		int32_t angle,
		const MouseWheelResponseCallback& response)>;

	virtual void SetMouseWheelCallback(const ProcessMouseWheelRequestCallback& requestProcessing) = 0;
};

} // namespace InputService
} // namespace TVRemoteScreenSDKCommunication
