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

#include <TVRemoteScreenSDKCommunication/ViewGeometryService/IViewGeometryServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/SocketIOTransport.h>

#include <TVRemoteScreenSDKCommunication/ServiceBase/SocketIO/ChannelInterface.h>

#include <memory>
#include <string>

namespace TVRemoteScreenSDKCommunication
{

namespace ViewGeometryService
{

class ViewGeometryServiceSocketIOClient : public IViewGeometryServiceClient
{
public:
	using TransportFW = Transport::TCPSocket;

	ViewGeometryServiceSocketIOClient() = default;
	~ViewGeometryServiceSocketIOClient() override = default;

	void StartClient(const std::string& destination) override;
	void StopClient(bool force) override;

	ServiceType GetServiceType() const override;
	const std::string& GetDestination() const override;

	// rpc call UpdateAreaOfInterest
	UpdateResult UpdateAreaOfInterest(

		const std::string& comId,
		const Rect& areaOfInterest) override;

	// rpc call UpdateVirtualDesktop
	UpdateResult UpdateVirtualDesktop(

		const std::string& comId,
		const VirtualDesktop& virtualDesktopGeometry) override;

private:
	std::string m_destination;
	std::unique_ptr<TransportFW::ChannelInterface> m_channel;
};

} // namespace ViewGeometryService

} // namespace TVRemoteScreenSDKCommunication