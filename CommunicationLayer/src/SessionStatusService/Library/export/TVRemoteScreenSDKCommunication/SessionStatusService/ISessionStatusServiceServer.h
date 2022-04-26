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

#include "GrabStrategy.h"

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/CallStatus.h>
#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/IServiceServer.h>

#include <string>
#include <functional>

namespace TVRemoteScreenSDKCommunication
{
namespace SessionStatusService
{

class ISessionStatusServiceServer : public IServiceServer
{
public:
	virtual ~ISessionStatusServiceServer() = default;

	using RemoteControllStartedResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using RemoteControlStartedCallback = std::function<void(
		const std::string& comId,
		const SessionStatusService::GrabStrategy strategy,
		const RemoteControllStartedResponseCallback& response)>;

	virtual void SetRemoteControlStartedCallback(const RemoteControlStartedCallback& requestProcessing) = 0;

	using RemoteControllStoppedResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using RemoteControlStoppedCallback = std::function<void(
		const std::string& comId,
		const RemoteControllStoppedResponseCallback& response)>;

	virtual void SetRemoteControlStoppedCallback(const RemoteControlStoppedCallback& requestProcessing) = 0;

	using TVSessionStartedResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using TVSessionStartedCallback = std::function<void(
		const std::string& comId,
		int32_t tvSessionID,
		int32_t tvSessionsCount,
		const TVSessionStartedResponseCallback& response)>;

	virtual void SetTVSessionStartedCallback(const TVSessionStartedCallback& requestProcessing) = 0;

	using TVSessionStoppedResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using TVSessionStoppedCallback = std::function<void(
		const std::string& comId,
		int32_t tvSessionID,
		int32_t tvSessionsCount,
		const TVSessionStoppedResponseCallback& response)>;

	virtual void SetTVSessionStoppedCallback(const TVSessionStoppedCallback& requestProcessing) = 0;
};

} // namespace SessionStatusService
} // namespace TVRemoteScreenSDKCommunication
