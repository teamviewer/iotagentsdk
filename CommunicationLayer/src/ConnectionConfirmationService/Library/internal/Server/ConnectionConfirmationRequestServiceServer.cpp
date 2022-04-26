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
#include "ConnectionConfirmationRequestServiceServer.h"

namespace TVRemoteScreenSDKCommunication
{
namespace ConnectionConfirmationService
{

ConnectionConfirmationRequestServiceServer::ConnectionConfirmationRequestServiceServer()
	: BaseType{ServiceType::ConnectionConfirmationRequest}
{
}

ConnectionConfirmationRequestServiceServer::~ConnectionConfirmationRequestServiceServer() = default;

void ConnectionConfirmationRequestServiceServer::SetRequestConnectionConfirmationCallback(
	const ConnectionConfirmationRequestCallback& connectionConfirmationRequestCallback)
{
	SetCallback<CBEnum::RequestConnectionConfirmation>(connectionConfirmationRequestCallback);
}

::grpc::Status ConnectionConfirmationRequestServiceServer::RequestConnectionConfirmation(
	::grpc::ServerContext* context,
	const ::tvconnectionconfirmationservice::RequestConnectionConfirmationRequest* request,
	::tvconnectionconfirmationservice::RequestConnectionConfirmationResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::RequestConnectionConfirmation,
		::tvconnectionconfirmationservice::RequestConnectionConfirmationRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			ConnectionType connectionType = ConnectionType::Undefined;
			switch (request.connectiontype())
			{
				case ::tvconnectionconfirmationservice::
					RequestConnectionConfirmationRequest_ConnectionType_InstantSupport:
					connectionType = ConnectionType::InstantSupport;
					break;
				case ::tvconnectionconfirmationservice::
					RequestConnectionConfirmationRequest_ConnectionType_Undefined:
					connectionType = ConnectionType::Undefined;
					break;
				default:;
			}

			callback(comId, connectionType, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

} // namespace ConnectionConfirmationService
} // namespace TVRemoteScreenSDKCommunication
