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

#include "ImageNotificationService.grpc.pb.h"

#include <TVRemoteScreenSDKCommunication/ImageNotificationService/IImageNotificationServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/AbstractServiceServer.h>

namespace TVRemoteScreenSDKCommunication
{
namespace ImageNotificationService
{

struct CallbacksMeta
{
	enum class Enum : size_t
	{
		UpdateImageDefinition,
		NotifyImageChanged,

		EnumSize
	};

	using Callbacks = std::tuple<
		IImageNotificationServiceServer::ProcessUpdateImageDefinitionRequestCallback,
		IImageNotificationServiceServer::ProcessNotifyImageChangedRequestCallback>;
};

class ImageNotificationServiceServer :
	public ServiceBase::AbstractServiceServer<
		IImageNotificationServiceServer,
		::tvimagenotificationservice::ImageNotificationService,
		CallbacksMeta>
{
public:
	ImageNotificationServiceServer();
	~ImageNotificationServiceServer() override;

	void SetUpdateImageDefinitionCallback(
		const ProcessUpdateImageDefinitionRequestCallback& requestProcessing) override;
	void SetNotifyImageChangedCallback(
		const ProcessNotifyImageChangedRequestCallback& requestProcessing) override;

	::grpc::Status UpdateImageDefinition(
		::grpc::ServerContext* context,
		const ::tvimagenotificationservice::ChangeImageDefinitionRequest* request,
		::tvimagenotificationservice::ChangeImageDefinitionResponse* response) override;

	::grpc::Status NotifiyImageChanged(
		::grpc::ServerContext* context,
		const ::tvimagenotificationservice::NotifyImageChangedRequest* request,
		::tvimagenotificationservice::NotifyImageChangedResponse* response) override;
};

} // namespace ImageService
} // namespace TVRemoteScreenSDKCommunication
