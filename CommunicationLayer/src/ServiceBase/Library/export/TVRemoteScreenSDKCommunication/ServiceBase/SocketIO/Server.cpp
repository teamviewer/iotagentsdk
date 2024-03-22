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
#include "Server.h"

#if defined(_WIN32) || defined(WINCE)
#include <winsock2.h>
typedef int socklen_t;
#ifdef WINCE
#pragma comment(lib, "coredll.lib")
#endif // _WINCE
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#include <iostream>

namespace TVRemoteScreenSDKCommunication
{

namespace Transport
{

namespace SocketIO
{

Server::Server(ServerFunctionMap functions)
	: m_serverFunctions(std::move(functions))
	, m_serverSocket(InvalidSocket)
	, m_endpoint(InvalidSocket)
	, m_socketSetup(QuerySockets())
{
}

Server::~Server()
{
	Shutdown();
}

Envelope Server::HandleRequest(Envelope request)
{
	auto registeredFunction = m_serverFunctions.find(request.functionId);

	Envelope response;
	response.comId = request.comId;
	response.functionId = request.functionId;
	response.data = std::make_shared<std::string>();

	Status logicStatus = registeredFunction != m_serverFunctions.cend() ?
		registeredFunction->second(request.comId, std::move(request.data), response.data) :
		Status{StatusCode::LOGIC_ERROR, "no function registered for id " + std::to_string(request.functionId)};

	response.statusCode = static_cast<decltype(response.statusCode)>(logicStatus.code());
	response.statusMessage = logicStatus.error_message();

	return response;
}

void Server::HandleConnections()
{
	bool keepRunning = true;
	socket_t serverSocket = InvalidSocket;
	socket_t endpoint = InvalidSocket;
	while (keepRunning)
	{
		{
			const std::lock_guard<std::mutex> lock{m_ioMutex};
			serverSocket = m_serverSocket;
		}
		keepRunning = serverSocket != InvalidSocket;

		if (!keepRunning)
		{
			return;
		}

		sockaddr_in client{};
		int clientAddrLength = sizeof(client);
		endpoint = ::accept(serverSocket, reinterpret_cast<sockaddr*>(&client), reinterpret_cast<socklen_t*>(&clientAddrLength));

		if (endpoint == InvalidSocket)
		{
			continue;
		}

		timeval timeout{};
		timeout.tv_sec = SendReceiveTimeout;

		if (::setsockopt(endpoint, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) != 0)
		{
			std::cerr << "Server::HandleConnections: setsockopt() failed " << GetLastSocketError() << std::endl;
			continue;
		}

		// write back endpoint
		{
			const std::lock_guard<std::mutex> lock{m_ioMutex};
			m_endpoint = endpoint;
		}

		while (keepRunning)
		{
			// send preamble
			Status preambleResult = ReceivePreamble(endpoint);
			if (preambleResult.code() == StatusCode::CONNECTION_CLOSED)
			{
				// Client has been disconnected. Waiting for another client.
				break;
			}

			if (!preambleResult.ok())
			{
				std::cerr << "Server::HandleConnections: receiving preamble failed; "
					<< preambleResult.error_message() << std::endl;
				break;
			}

			// receive request envelope
			Envelope envelope{};
			Status receiveResult = ReceiveEnvelope(endpoint, envelope);

			if (!receiveResult.ok())
			{
				std::cerr << "Server::HandleConnections: Error receiving envelope; "
					<< receiveResult.error_message() << std::endl;
				break;
			}

			Status sendResult = Status::OK;
			{
				Envelope response = HandleRequest(std::move(envelope));
				sendResult = SendEnvelope(endpoint, response);
			}

			if (!sendResult.ok())
			{
				std::cerr << "Server::HandleConnections: Error sending envelope; "
					<< sendResult.error_message() << std::endl;
				break;
			}

			{
				const std::lock_guard<std::mutex> lock(m_ioMutex);
				endpoint = m_endpoint;
				serverSocket = m_serverSocket;
			}
			keepRunning = serverSocket != InvalidSocket;
		}

		CloseSocket(endpoint);
		endpoint = InvalidSocket;
		{
			const std::lock_guard<std::mutex> lock(m_ioMutex);
			m_endpoint = endpoint;
		}
	}
}

bool Server::Start(const std::string& locationUri)
{
	sockaddr_in serverAddress{};

	if (!ParseLocationUri(locationUri, serverAddress.sin_addr, serverAddress.sin_port))
	{
		return false;
	}
	serverAddress.sin_family = AF_INET;

	Shutdown();

	const std::lock_guard<std::mutex> lock{m_ioMutex};

	// setup socket
	ResetLastSocketError();
	m_serverSocket = ::socket(AF_INET, SocketType, 0);
	if (m_serverSocket == InvalidSocket)
	{
		std::cerr << "Server::Startup: socket() failed " << GetLastSocketError() << std::endl;
		return false;
	}

#if defined(_WIN32) || defined(_WINCE)
	constexpr int optName = SO_REUSEADDR;
	using socketopt_t = char;
#else
	constexpr int optName = SO_REUSEADDR;
	using socketopt_t = int;
#endif

	int socketOptions = 1;
	ResetLastSocketError();
	if (::setsockopt(m_serverSocket, SOL_SOCKET, optName, reinterpret_cast<socketopt_t*>(&socketOptions), sizeof(socketOptions)) != 0)
	{
		std::cerr << "Server::Startup: setsockopt() failed " << GetLastSocketError() << std::endl;
		CloseSocket(m_serverSocket);
		m_serverSocket = InvalidSocket;
		return false;
	}

	// bind address and listen
	ResetLastSocketError();
	if (::bind(m_serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0)
	{
		std::cerr << "Server::Startup: setsockopt() failed " << GetLastSocketError() << std::endl;
		CloseSocket(m_serverSocket);
		m_serverSocket = InvalidSocket;
		return false;
	}

	ResetLastSocketError();
	if (::listen(m_serverSocket, MaxBacklogSize) < 0)
	{
		std::cerr << "Server::Startup: setsockopt() failed " << GetLastSocketError() << std::endl;
		CloseSocket(m_serverSocket);
		m_serverSocket = InvalidSocket;
		return false;
	}

	m_communicationThread = std::thread([this]()
	{
		HandleConnections();
	});

	return true;
}

void Server::Shutdown()
{
	{
		const std::lock_guard<std::mutex> lock{m_ioMutex};

		CloseSocket(m_endpoint);
		m_endpoint = InvalidSocket;

		CloseSocket(m_serverSocket);
		m_serverSocket = InvalidSocket;
	}

	if (m_communicationThread.joinable())
	{
		m_communicationThread.join();
	}
}

} // namespace SocketIO

} // namespace Transport

} // namespace TVRemoteScreenSDKCommunication
