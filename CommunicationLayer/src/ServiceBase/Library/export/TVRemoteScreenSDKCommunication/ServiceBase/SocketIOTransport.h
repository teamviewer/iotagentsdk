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

#include "SocketIO/ChannelInterface.h"
#include "SocketIO/ClientContext.h"
#include "SocketIO/Server.h"
#include "SocketIO/ServerContext.h"
#include "SocketIO/Status.h"
#include "SocketIO/Service.h"

#include <memory>
#include <string>

namespace TVRemoteScreenSDKCommunication
{

namespace Transport
{

struct TCPSocket final
{
	using ChannelInterface = SocketIO::ChannelInterface;
	using ClientContext    = SocketIO::ClientContext;
	using Server           = SocketIO::Server;
	using ServerContext    = SocketIO::ServerContext;
	using Service          = SocketIO::Service;
	using Status           = SocketIO::Status;
	using StatusCode       = SocketIO::StatusCode;

	static std::shared_ptr<ChannelInterface> CreateChannel(const std::string& destination);

	/**
	 * @brief CreateAndStartSyncServer creates and starts a synchronous SocketIO server with the given parameters.
	 * Synchronous server means its client wait synchronously until the server responds.
	 * @param locationUri location of the server (IP address and port, e.g. 127.0.0.1:9003).
	 * @return server instance if creating and starting was successful
	 */
	static std::unique_ptr<Server> CreateAndStartSyncServer(
		const std::string& locationUri,
		Service* service);
};

} // namespace Transport

} // namespace TVRemoteScreenSDKCommunication
