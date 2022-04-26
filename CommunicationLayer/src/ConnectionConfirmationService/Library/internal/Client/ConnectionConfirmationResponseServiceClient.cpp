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
#include "ConnectionConfirmationResponseServiceClient.h"

#include <TVRemoteScreenSDKCommunication/ServiceBase/ClientErrorMessage.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/RequestMetadata.h>

#include <grpcpp/create_channel.h>

namespace TVRemoteScreenSDKCommunication
{
namespace ConnectionConfirmationService
{

ConnectionConfirmationResponseServiceClient::ConnectionConfirmationResponseServiceClient()
	: BaseType{ServiceType::ConnectionConfirmationResponse}
{
}

CallStatus ConnectionConfirmationResponseServiceClient::ConfirmConnectionRequest(
	const std::string& comId,
	ConnectionType connectionType,
	ConnectionUserConfirmation confirmation)
{
	::tvconnectionconfirmationservice::
		ConfirmConnectionRequestRequest::ConnectionType type =
			::tvconnectionconfirmationservice::
				ConfirmConnectionRequestRequest_ConnectionType_Undefined;
	switch (connectionType)
	{
		case ConnectionType::InstantSupport:
			type = ::tvconnectionconfirmationservice::
				ConfirmConnectionRequestRequest_ConnectionType_InstantSupport;
			break;
		case ConnectionType::Undefined:
			type = ::tvconnectionconfirmationservice::
				ConfirmConnectionRequestRequest_ConnectionType_Undefined;
			break;
	}

	::tvconnectionconfirmationservice::ConfirmConnectionRequestRequest::
		ConnectionUserConfirmation confirm =
			::tvconnectionconfirmationservice::
				ConfirmConnectionRequestRequest_ConnectionUserConfirmation_NoResponse;
	switch (confirmation)
	{
		case ConnectionUserConfirmation::Accepted:
			confirm = ::tvconnectionconfirmationservice::
				ConfirmConnectionRequestRequest_ConnectionUserConfirmation_Accepted;
			break;
		case ConnectionUserConfirmation::Denied:
			confirm = ::tvconnectionconfirmationservice::
				ConfirmConnectionRequestRequest_ConnectionUserConfirmation_Denied;
			break;
		case ConnectionUserConfirmation::NoResponse:
			confirm = ::tvconnectionconfirmationservice::
				ConfirmConnectionRequestRequest_ConnectionUserConfirmation_NoResponse;
			break;
	}

	struct Meta: DefaultMeta
	{
		using Request =
			::tvconnectionconfirmationservice::ConfirmConnectionRequestRequest;
		using Response =
			::tvconnectionconfirmationservice::ConfirmConnectionRequestResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			::tvconnectionconfirmationservice::
				ConfirmConnectionRequestRequest::ConnectionType type,
			::tvconnectionconfirmationservice::
				ConfirmConnectionRequestRequest::ConnectionUserConfirmation confirm)
		{
			request.set_connectiontype(type);
			request.set_confirmation(confirm);

			return stub.ConfirmConnectionRequest(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, type, confirm);
}

} // namespace ConnectionConfirmationService
} // namespace TVRemoteScreenSDKCommunication
