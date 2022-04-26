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

#include "ViewGeometryService.grpc.pb.h"

#include <TVRemoteScreenSDKCommunication/ViewGeometryService/IViewGeometryServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/AbstractServiceServer.h>

namespace TVRemoteScreenSDKCommunication
{
namespace ViewGeometryService
{

struct CallbacksMeta
{
	enum class Enum : size_t
	{
		UpdateAreaOfInterest,
		UpdateVirtualDesktop,

		EnumSize
	};

	using Callbacks = std::tuple<
		IViewGeometryServiceServer::UpdateAreaOfInterestCallback,
		IViewGeometryServiceServer::UpdateVirtualDesktopCallback>;
};

class ViewGeometryServiceServer :
	public ServiceBase::AbstractServiceServer<
		IViewGeometryServiceServer,
		::tvviewgeometryservice::ViewGeometryService,
		CallbacksMeta>
{
public:
	ViewGeometryServiceServer();
	~ViewGeometryServiceServer() override;

	void SetUpdateAreaOfInterestCallback(
		const UpdateAreaOfInterestCallback& requestProcessing) override;

	void SetUpdateVirtualDesktopCallback(
		const UpdateVirtualDesktopCallback& requestProcessing) override;

	::grpc::Status UpdateAreaOfInterest(
		::grpc::ServerContext* context,
		const ::tvviewgeometryservice::UpdateAreaOfInterestRequest* request,
		::tvviewgeometryservice::UpdateAreaOfInterestResponse* response) override;

	::grpc::Status UpdateVirtualDesktop(
		::grpc::ServerContext* context,
		const ::tvviewgeometryservice::UpdateVirtualDesktopRequest* request,
		::tvviewgeometryservice::UpdateVirtualDesktopResponse* response) override;
};

} // namespace ViewGeometryService
} // namespace TVRemoteScreenSDKCommunication
