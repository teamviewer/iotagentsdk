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

#include "ImageService.grpc.pb.h"

#include <TVRemoteScreenSDKCommunication/ImageService/IImageServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/AbstractServiceServer.h>

namespace TVRemoteScreenSDKCommunication
{
namespace ImageService
{

struct CallbacksMeta
{
	enum class Enum : size_t
	{
		ImageUpdate,
		ImageDefinition,

		EnumSize
	};

	using Callbacks = std::tuple<
		IImageServiceServer::ProcessImageUpdateRequestCallback,
		IImageServiceServer::ProcessImageDefinitionRequestCallback>;
};

class ImageServiceServer :
	public ServiceBase::AbstractServiceServer<
		IImageServiceServer,
		::tvimageservice::ImageService,
		CallbacksMeta>
{
public:
	ImageServiceServer();
	~ImageServiceServer() override;

	void SetImageUpdateCallback(
		const ProcessImageUpdateRequestCallback& requestProcessing) override;

	void SetImageDefinitionCallback(
		const ProcessImageDefinitionRequestCallback& requestProcessing) override;

	::grpc::Status UpdateImage(
		::grpc::ServerContext* context,
		::grpc::ServerReader<::tvimageservice::GrabResult>* reader,
		::tvimageservice::ImageUpdateResponse* response) override;

	::grpc::Status UpdateImageDefinition(
		::grpc::ServerContext* context,
		const ::tvimageservice::ImageDefinitionRequest* request,
		::tvimageservice::ImageDefinitionResponse* response) override;
};

} // namespace ImageService
} // namespace TVRemoteScreenSDKCommunication
