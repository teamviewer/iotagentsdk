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

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/CallStatus.h>
#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/IServiceServer.h>

#include "AccessControl.h"

#include <functional>

namespace TVRemoteScreenSDKCommunication
{
namespace AccessControlService
{

class IAccessControlInServiceServer : public IServiceServer
{
public:
	virtual ~IAccessControlInServiceServer() = default;

	using ConfirmationReplyResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessConfirmationReplyRequestCallback = std::function<void(
		const std::string& comId,
		AccessControl feature,
		bool confirmed,
		const ConfirmationReplyResponseCallback& response)>;
	virtual void SetConfirmationReplyCallback(const ProcessConfirmationReplyRequestCallback& requestProcessing) = 0;

	using GetAccessResponseCallback = std::function<void(const CallStatus& callStatus, Access access)>;
	using ProcessGetAccessRequestCallback = std::function<void(
		const std::string& comId,
		AccessControl feature,
		const GetAccessResponseCallback& response)>;
	virtual void SetGetAccessCallback(const ProcessGetAccessRequestCallback& requestProcessing) = 0;

	using SetAccessResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessSetAccessRequestCallback = std::function<void(
		const std::string& comId,
		AccessControl feature,
		Access access,
		const SetAccessResponseCallback& response)>;
	virtual void SetSetAccessCallback(const ProcessSetAccessRequestCallback& requestProcessing) = 0;

};

} // namespace AccessControlService
} // namespace TVRemoteScreenSDKCommunication
