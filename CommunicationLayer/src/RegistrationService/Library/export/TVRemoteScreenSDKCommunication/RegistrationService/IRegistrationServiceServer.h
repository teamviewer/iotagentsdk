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

#include "ServiceInformation.h"

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/CallStatus.h>
#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/IServiceServer.h>

#include <string>
#include <functional>
#include <vector>

namespace TVRemoteScreenSDKCommunication
{
namespace RegistrationService
{

class IRegistrationServiceServer : public IServiceServer
{
public:
	virtual ~IRegistrationServiceServer() = default;

	using ExchangeVersionResponseCallback = std::function<void(const std::string& ownVersionNumber, const CallStatus& callStatus)>;
	using ProcessExchangeVersionRequestCallback = std::function<void(const std::string& receivedVersion, const ExchangeVersionResponseCallback& response)>;

	virtual void SetExchangeVersionCallback(const ProcessExchangeVersionRequestCallback& requestProcessing) = 0;

	using DiscoverResponseCallback = std::function<void(const std::string& comId, const std::vector<ServiceInformation>& services, const CallStatus& callStatus)>;
	using ProcessDiscoverRequestCallback = std::function<void(const std::string& communicationVersion, const DiscoverResponseCallback& response)>;
	virtual void SetDiscoverCallback(const ProcessDiscoverRequestCallback& requestProcessing) = 0;

	using RegisterResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessRegisterRequestCallback = std::function<void(const std::string& comId, ServiceType id, const std::string& location, const RegisterResponseCallback& response)>;
	virtual void SetRegisterCallback(const ProcessRegisterRequestCallback& requestProcessing) = 0;
};

} // namespace RegistrationService
} // namespace TVRemoteScreenSDKCommunication
