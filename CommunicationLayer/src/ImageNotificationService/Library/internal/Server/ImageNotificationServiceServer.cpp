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
#include "ImageNotificationServiceServer.h"

namespace TVRemoteScreenSDKCommunication
{
namespace ImageNotificationService
{

ImageNotificationServiceServer::ImageNotificationServiceServer()
	: BaseType{ServiceType::ImageNotification}
{
}

ImageNotificationServiceServer::~ImageNotificationServiceServer() = default;

void ImageNotificationServiceServer::SetUpdateImageDefinitionCallback(
	const ProcessUpdateImageDefinitionRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::UpdateImageDefinition>(requestProcessing);
}

void ImageNotificationServiceServer::SetNotifyImageChangedCallback(
	const ProcessNotifyImageChangedRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::NotifyImageChanged>(requestProcessing);
}

::grpc::Status ImageNotificationServiceServer::UpdateImageDefinition(
	::grpc::ServerContext* context,
	const ::tvimagenotificationservice::ChangeImageDefinitionRequest* request,
	::tvimagenotificationservice::ChangeImageDefinitionResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::UpdateImageDefinition,
		::tvimagenotificationservice::ChangeImageDefinitionRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			callback(
				comId,
				request.imagesourcetitle(),
				request.width(),
				request.height(),
				responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status ImageNotificationServiceServer::NotifiyImageChanged(
	::grpc::ServerContext* context,
	const ::tvimagenotificationservice::NotifyImageChangedRequest* request,
	::tvimagenotificationservice::NotifyImageChangedResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::NotifyImageChanged,
		::tvimagenotificationservice::NotifyImageChangedRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			callback(
				comId,
				request.x(),
				request.y(),
				request.width(),
				request.height(),
				responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

} // namespace ImageNotificationService
} // namespace TVRemoteScreenSDKCommunication
