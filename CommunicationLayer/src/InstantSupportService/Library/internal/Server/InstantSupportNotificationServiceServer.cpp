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
#include "InstantSupportNotificationServiceServer.h"

namespace TVRemoteScreenSDKCommunication
{
namespace InstantSupportService
{

InstantSupportNotificationServiceServer::InstantSupportNotificationServiceServer()
	: BaseType{ServiceType::InstantSupportNotification}
{
}

InstantSupportNotificationServiceServer::~InstantSupportNotificationServiceServer() = default;

void InstantSupportNotificationServiceServer::SetInstantSupportErrorNotificationCallback(
	const InstantSupportErrorNotificationCallback& errorHandlerCallback)
{
	SetCallback<CBEnum::Error>(errorHandlerCallback);
}

void InstantSupportNotificationServiceServer::SetInstantSupportModifiedNotificationCallback(
	const InstantSupportModifiedNotificationCallback& modifyHandlerCallback)
{
	SetCallback<CBEnum::Modified>(modifyHandlerCallback);
}

::grpc::Status InstantSupportNotificationServiceServer::NotifyInstantSupportError(
	::grpc::ServerContext* context,
	const ::tvinstantsupportservice::ErrorNotificationRequest* request,
	::tvinstantsupportservice::ErrorNotificationResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::Error,
		::tvinstantsupportservice::ErrorNotificationRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			InstantSupportError errorCode = InstantSupportError::InternalError;
			switch (request.errorcode())
			{
				case ::tvinstantsupportservice::
						ErrorNotificationRequest_ErrorCode_InvalidToken:
					errorCode = InstantSupportError::InvalidToken;
					break;
				case ::tvinstantsupportservice::
						ErrorNotificationRequest_ErrorCode_InvalidGroup:
					errorCode = InstantSupportError::InvalidGroup;
					break;
				case ::tvinstantsupportservice::
						ErrorNotificationRequest_ErrorCode_InvalidSessionCode:
					errorCode = InstantSupportError::InvalidSessionCode;
					break;
				case ::tvinstantsupportservice::
						ErrorNotificationRequest_ErrorCode_Busy:
					errorCode = InstantSupportError::Busy;
					break;
				case::tvinstantsupportservice::
						ErrorNotificationRequest_ErrorCode_InvalidEmail:
					errorCode = InstantSupportError::InvalidEmail;
					break;
				case ::tvinstantsupportservice::
						ErrorNotificationRequest_ErrorCode_InternalError:
				default:;
			}

			callback(comId, errorCode, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status InstantSupportNotificationServiceServer::NotifyInstantSupportModified(
	::grpc::ServerContext* context,
	const ::tvinstantsupportservice::ModifiedNotificationRequest* request,
	::tvinstantsupportservice::ModifiedNotificationResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::Modified,
		::tvinstantsupportservice::ModifiedNotificationRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			InstantSupportData data;
			data.sessionCode = request.sessioncode();
			data.name = request.name();
			data.description = request.description();

			switch (request.state())
			{
				case ::tvinstantsupportservice::
						ModifiedNotificationRequest_InstantSupportState_Open:
					data.state = InstantSupportState::Open;
					break;
				case ::tvinstantsupportservice::
						ModifiedNotificationRequest_InstantSupportState_Closed:
					data.state = InstantSupportState::Closed;
					break;
				case ::tvinstantsupportservice::
						ModifiedNotificationRequest_InstantSupportState_Undefined:
				default:
					data.state = InstantSupportState::Undefined;
			}

			callback(comId, data, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

} // namespace InstantSupportService
} // namespace TVRemoteScreenSDKCommunication
