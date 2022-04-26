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
#include "InstantSupportServiceClient.h"

namespace TVRemoteScreenSDKCommunication
{
namespace InstantSupportService
{

InstantSupportServiceClient::InstantSupportServiceClient()
	: BaseType{ServiceType::InstantSupport}
{
}

CallStatus InstantSupportServiceClient::RequestInstantSupport(
	const std::string& comId,
	const std::string& accessToken,
	const std::string& name,
	const std::string& group,
	const std::string& description,
	const std::string& sessionCode,
	const std::string& email)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvinstantsupportservice::RequestInstantSupportRequest;
		using Response = ::tvinstantsupportservice::RequestInstantSupportResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			const std::string& accessToken,
			const std::string& name,
			const std::string& group,
			const std::string& description,
			const std::string& sessionCode,
			const std::string& email)
		{
			request.set_accesstoken(accessToken);
			request.set_name(name);
			request.set_group(group);
			request.set_description(description);
			request.set_sessioncode(sessionCode);
			request.set_email(email);

			return stub.RequestInstantSupport(&context, request, &response);
		}
	};

	return ClientCall<Meta>(
		comId,
		accessToken,
		name,
		group,
		description,
		sessionCode,
		email);
}

} // namespace InstantSupportService
} // namespace TVRemoteScreenSDKCommunication
