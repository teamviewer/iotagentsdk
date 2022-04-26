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
#include "ConnectionConfirmationResponseServiceServer.h"

namespace TVRemoteScreenSDKCommunication
{
namespace ConnectionConfirmationService
{

ConnectionConfirmationResponseServiceServer::ConnectionConfirmationResponseServiceServer()
	: BaseType{ServiceType::ConnectionConfirmationResponse}
{
}

ConnectionConfirmationResponseServiceServer::~ConnectionConfirmationResponseServiceServer() = default;

void ConnectionConfirmationResponseServiceServer::SetConfirmConnectionRequestCallback(
	const ConfirmConnectionRequestCallback& confirmConnectionRequestCallback)
{
	SetCallback<CBEnum::ConfirmConnection>(confirmConnectionRequestCallback);
}

::grpc::Status ConnectionConfirmationResponseServiceServer::ConfirmConnectionRequest(
	::grpc::ServerContext* context,
	const ::tvconnectionconfirmationservice::ConfirmConnectionRequestRequest* request,
	::tvconnectionconfirmationservice::ConfirmConnectionRequestResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::ConfirmConnection,
		::tvconnectionconfirmationservice::ConfirmConnectionRequestRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			ConnectionUserConfirmation confirmation = ConnectionUserConfirmation::NoResponse;
			switch (request.confirmation())
			{
				case ::tvconnectionconfirmationservice::
					ConfirmConnectionRequestRequest_ConnectionUserConfirmation_Accepted:
					confirmation = ConnectionUserConfirmation::Accepted;
					break;
				case ::tvconnectionconfirmationservice::
					ConfirmConnectionRequestRequest_ConnectionUserConfirmation_Denied:
					confirmation = ConnectionUserConfirmation::Denied;
					break;
				case ::tvconnectionconfirmationservice::
					ConfirmConnectionRequestRequest_ConnectionUserConfirmation_NoResponse:
					confirmation = ConnectionUserConfirmation::NoResponse;
					break;
				default:;
			}

			ConnectionType connectionType = ConnectionType::Undefined;
			switch (request.connectiontype())
			{
				case ::tvconnectionconfirmationservice::
					ConfirmConnectionRequestRequest_ConnectionType_InstantSupport:
					connectionType = ConnectionType::InstantSupport;
					break;
				case ::tvconnectionconfirmationservice::
					ConfirmConnectionRequestRequest_ConnectionType_Undefined:
					connectionType = ConnectionType::Undefined;
					break;
				default:;
			}

			callback(
				comId,
				connectionType,
				confirmation,
				responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

} // namespace ConnectionConfirmationService
} // namespace TVRemoteScreenSDKCommunication
