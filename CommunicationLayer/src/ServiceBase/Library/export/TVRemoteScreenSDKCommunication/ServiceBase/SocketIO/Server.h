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

#include "Socket.h"

#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

namespace TVRemoteScreenSDKCommunication
{

namespace Transport
{

namespace SocketIO
{

class Server final
{
public:
	using ServerFunction = std::function<Status(
		const std::string& comId,
		std::shared_ptr<std::string> request,
		std::shared_ptr<std::string> response)>;

	using ServerFunctionMap = std::unordered_map<int64_t, ServerFunction>;

	explicit Server(ServerFunctionMap functions);
	~Server();

	bool Start(const std::string& locationUri);
	void Shutdown();

private:
	const ServerFunctionMap m_serverFunctions;

	Envelope HandleRequest(Envelope request);

	std::mutex m_ioMutex;
	socket_t m_serverSocket;
	socket_t m_endpoint;

	std::thread m_communicationThread;

	std::shared_ptr<SocketSetup> m_socketSetup;

	void HandleConnections();
};

} // namespace SocketIO

} // namespace Transport

} // namespace TVRemoteScreenSDKCommunication
