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
#include "ImageNotificationServiceClient.h"

namespace TVRemoteScreenSDKCommunication
{
namespace ImageNotificationService
{

ImageNotificationServiceClient::ImageNotificationServiceClient()
	: BaseType{ServiceType::ImageNotification}
{
}

CallStatus ImageNotificationServiceClient::UpdateImageDefinition(
	const std::string& comId,
	const std::string& imageSourceTitle,
	int32_t width,
	int32_t height)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvimagenotificationservice::ChangeImageDefinitionRequest;
		using Response = ::tvimagenotificationservice::ChangeImageDefinitionResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			const std::string& imageSourceTitle,
			int32_t width,
			int32_t height)
		{
			request.set_imagesourcetitle(imageSourceTitle);
			request.set_width(width);
			request.set_height(height);

			return stub.UpdateImageDefinition(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, imageSourceTitle, width, height);
}

CallStatus ImageNotificationServiceClient::NotifyImageChanged(
	const std::string& comId,
	int32_t x,
	int32_t y,
	int32_t width,
	int32_t height)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvimagenotificationservice::NotifyImageChangedRequest;
		using Response = ::tvimagenotificationservice::NotifyImageChangedResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			int32_t x,
			int32_t y,
			int32_t width,
			int32_t height)
		{
			request.set_x(x);
			request.set_y(y);
			request.set_width(width);
			request.set_height(height);

			return stub.NotifiyImageChanged(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, x, y, width, height);
}

} // namespace ImageNotificationService
} // namespace TVRemoteScreenSDKCommunication
