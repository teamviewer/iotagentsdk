//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2023 TeamViewer Germany GmbH                                     //
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

#include "Server.h"
#include "ServerContext.h"
#include "Status.h"

#include <google/protobuf/message_lite.h>

#include <memory>
#include <type_traits>

#define TV_SERVICE_STUB_METHOD(NAME, NAMESPACE)                                  \
	Transport::SocketIO::Status NAME(                                            \
		Transport::SocketIO::ClientContext* context,                             \
		const ::NAMESPACE::NAME##Request& request,                               \
		::NAMESPACE::NAME##Response* response)                                   \
		{                                                                        \
			return channel->Call(*context, Function_##NAME, request, *response); \
		}

#define TV_SERVICE_SERVICE_METHOD(NAME, NAMESPACE)        \
	virtual Transport::SocketIO::Status NAME(             \
		Transport::SocketIO::ServerContext* context,      \
		const ::NAMESPACE::NAME##Request* request,        \
		::NAMESPACE::NAME##Response* response) = 0;

#define TV_SERVICE_REGISTER_METHOD(NAME) \
	functions[Function_##NAME] = MakeServerFunction(&Service::NAME);

namespace TVRemoteScreenSDKCommunication
{

namespace Transport
{

namespace SocketIO
{

class Service
{
public:
	virtual ~Service() = default;

	Server::ServerFunctionMap RegisteredFunctions()
	{
		Server::ServerFunctionMap functions;
		Register(functions);
		return functions;
	}
protected:
	virtual void Register(Server::ServerFunctionMap& functions) = 0;

	template<
		typename Request,
		typename Response,
		typename Child>
	Server::ServerFunction MakeServerFunction(
		Status(Child::*serviceFunction)(ServerContext* context, Request* request, Response* response))
	{
		static_assert(
			std::is_base_of<::google::protobuf::MessageLite, Request>::value,
			"Request must inherit from ::google::protobuf::MessageLite");
		static_assert(std::is_base_of<::google::protobuf::MessageLite, Response>::value,
			"Response must inherit from ::google::protobuf::MessageLite");
		static_assert(std::is_base_of<Service, Child>::value,
			"Method must be of Transport::SocketIO::Service inherited class");

		return [this, serviceFunction](
				const std::string& comId,
				std::shared_ptr<std::string> requestRaw,
				std::shared_ptr<std::string> responseRaw
				) -> Status
			{
				typename std::remove_const<Request>::type request{};
				if (!request.ParseFromString(*requestRaw))
				{
					return {StatusCode::IO_ERROR, "error parsing request"};
				}
				requestRaw->clear();

				ServerContext context{comId};

				Response response{};
				Status status = (static_cast<Child*>(this)->*serviceFunction)(&context, &request, &response);
				if (!status.ok())
				{
					return status;
				}

				if (!response.SerializeToString(responseRaw.get()))
				{
					return {StatusCode::LOGIC_ERROR, "response serialization failed"};
				}
				return status;
			};
	}
};

} // namespace SocketIO

} // namespace Transport

} // namespace TVRemoteScreenSDKCommunication
