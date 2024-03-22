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

#include "ClientContext.h"
#include "Socket.h"
#include "Status.h"

#include <google/protobuf/message_lite.h>

#include <cstdint>
#include <string>
#include <mutex>

namespace TVRemoteScreenSDKCommunication
{

namespace Transport
{

namespace SocketIO
{

class ChannelInterface final
{
public:
	explicit ChannelInterface(std::string location);
	~ChannelInterface();

	Status Call(
		const std::string& comId,
		int64_t functionId,
		std::shared_ptr<std::string>&& request,
		std::shared_ptr<std::string>& response);

	Status Call( // TODO REMOVE
		const ClientContext& context,
		int64_t functionId,
		const ::google::protobuf::MessageLite& request,
		::google::protobuf::MessageLite& response);

	Status Call(
		const std::string& comId,
		int64_t functionId,
		const ::google::protobuf::MessageLite& request,
		::google::protobuf::MessageLite& response);


private:
	Status Connect();

	std::shared_ptr<SocketSetup> m_socketSetup;

	const std::string m_location;

	std::mutex m_ioMutex;
	socket_t m_clientSocket;
};

} // namespace SocketIO

} // namespace Transport

} // namespace TVRemoteScreenSDKCommunication
