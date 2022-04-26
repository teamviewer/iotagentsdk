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
#include "InstantSupportNotificationServiceClient.h"

namespace TVRemoteScreenSDKCommunication
{
namespace InstantSupportService
{

InstantSupportNotificationServiceClient::InstantSupportNotificationServiceClient()
	: BaseType{ServiceType::InstantSupportNotification}
{
}

CallStatus InstantSupportNotificationServiceClient::NotifyInstantSupportError(
	const std::string& comId,
	InstantSupportError errorCode)
{
	::tvinstantsupportservice::ErrorNotificationRequest_ErrorCode code =
		::tvinstantsupportservice::ErrorNotificationRequest_ErrorCode_InternalError;

	switch (errorCode)
	{
		case InstantSupportError::InvalidToken:
			code = ::tvinstantsupportservice::
				ErrorNotificationRequest_ErrorCode_InvalidToken;
			break;
		case InstantSupportError::InvalidGroup:
			code = ::tvinstantsupportservice::
				ErrorNotificationRequest_ErrorCode_InvalidGroup;
			break;
		case InstantSupportError::InvalidSessionCode:
			code = ::tvinstantsupportservice::
				ErrorNotificationRequest_ErrorCode_InvalidSessionCode;
			break;
		case InstantSupportError::Busy:
			code = ::tvinstantsupportservice::ErrorNotificationRequest_ErrorCode_Busy;
			break;
		case InstantSupportError::InvalidEmail:
			code = ::tvinstantsupportservice::
				ErrorNotificationRequest_ErrorCode_InvalidEmail;
			break;
		case InstantSupportError::InternalError:
			break;
	}

	struct Meta: DefaultMeta
	{
		using Request = ::tvinstantsupportservice::ErrorNotificationRequest;
		using Response = ::tvinstantsupportservice::ErrorNotificationResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			::tvinstantsupportservice::ErrorNotificationRequest_ErrorCode code)
		{
			request.set_errorcode(code);

			return stub.NotifyInstantSupportError(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, code);
}

CallStatus InstantSupportNotificationServiceClient::NotifyInstantSupportModified(
	const std::string& comId,
	InstantSupportData data)
{
	::tvinstantsupportservice::ModifiedNotificationRequest_InstantSupportState state =
		::tvinstantsupportservice::
			ModifiedNotificationRequest_InstantSupportState_Undefined;
	switch (data.state)
	{
		case InstantSupportState::Open:
			state = ::tvinstantsupportservice::
				ModifiedNotificationRequest_InstantSupportState_Open;
			break;
		case InstantSupportState::Closed:
			state = ::tvinstantsupportservice::
				ModifiedNotificationRequest_InstantSupportState_Closed;
			break;
		case InstantSupportState::Undefined:
			break;
	}

	struct Meta: DefaultMeta
	{
		using Request = ::tvinstantsupportservice::ModifiedNotificationRequest;
		using Response = ::tvinstantsupportservice::ModifiedNotificationResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			const std::string& sessionCode,
			const std::string& name,
			const std::string& description,
			::tvinstantsupportservice::
				ModifiedNotificationRequest_InstantSupportState state)
		{
			request.set_sessioncode(sessionCode);
			request.set_name(name);
			request.set_description(description);
			request.set_state(state);
			return stub.NotifyInstantSupportModified(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, data.sessionCode, data.name, data.description, state);
}

} // namespace InstantSupportService
} // namespace TVRemoteScreenSDKCommunication
