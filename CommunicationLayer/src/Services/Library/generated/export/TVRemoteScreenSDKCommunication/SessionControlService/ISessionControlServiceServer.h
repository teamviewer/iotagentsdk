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
#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/IServiceServer.h>

#include <TVRemoteScreenSDKCommunication/SessionControlService/ControlMode.h>

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace TVRemoteScreenSDKCommunication
{

namespace SessionControlService
{

class ISessionControlServiceServer : public IServiceServer
{
public:
	virtual ~ISessionControlServiceServer() = default;

	// rpc call ChangeControlMode
	using ChangeControlModeResponseCallback = std::function<void(

		const CallStatus& callStatus)>;
	using ProcessChangeControlModeRequestCallback = std::function<void(

		const std::string& comId,
		ControlMode controlMode,
		const ChangeControlModeResponseCallback& response)>;
	virtual void SetChangeControlModeCallback(const ProcessChangeControlModeRequestCallback& requestProcessing) = 0;

	// rpc call CloseRc
	using CloseRcResponseCallback = std::function<void(

		const CallStatus& callStatus)>;
	using ProcessCloseRcRequestCallback = std::function<void(

		const std::string& comId,
		const CloseRcResponseCallback& response)>;
	virtual void SetCloseRcCallback(const ProcessCloseRcRequestCallback& requestProcessing) = 0;

	// rpc call GetRunningTVSessions
	using GetRunningTVSessionsResponseCallback =
		std::function<void(const CallStatus& callStatus, const std::vector<int32_t>& tvSessionIDs)>;
	using ProcessGetRunningTVSessionsRequestCallback = std::function<void(

		const std::string& comId,
		const GetRunningTVSessionsResponseCallback& response)>;
	virtual void SetGetRunningTVSessionsCallback(const ProcessGetRunningTVSessionsRequestCallback& requestProcessing) = 0;
};

} // namespace SessionControlService

} // namespace TVRemoteScreenSDKCommunication