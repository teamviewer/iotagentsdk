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
#include "ChannelInterface.h"

#include <TVRemoteScreenSDKCommunication/ServiceBase/RequestMetadata.h>

#include "Envelope.pb.h"

#include <thread>
#include <iostream>

#if defined(_WIN32) || defined(_WINCE)
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

namespace TVRemoteScreenSDKCommunication
{

namespace Transport
{

namespace SocketIO
{

ChannelInterface::ChannelInterface(std::string location)
	: m_socketSetup(QuerySockets())
	, m_location(std::move(location))
	, m_clientSocket(InvalidSocket)
{

}

ChannelInterface::~ChannelInterface()
{
	std::lock_guard<std::mutex> lock{m_ioMutex};
	CloseSocket(m_clientSocket);
	m_clientSocket = InvalidSocket;
}

Status ChannelInterface::Call(
	const std::string& comId,
	int64_t functionId,
	std::shared_ptr<std::string>&& request,
	std::shared_ptr<std::string>& response)
{
	std::lock_guard<std::mutex> lock(m_ioMutex);

	Status returnStatus{StatusCode::IO_ERROR, "ChannelInterface::Call: unknown IO error"};

	int retryCounter = 10;
	Envelope envelopeToSend;

	envelopeToSend.comId = comId;
	envelopeToSend.functionId = functionId;
	envelopeToSend.data.swap(request);

	while (retryCounter > 0)
	{
		retryCounter--;

		if (m_clientSocket == InvalidSocket)
		{
			returnStatus = Connect();
		}

		if (m_clientSocket != InvalidSocket)
		{
			// try sending preamble
			returnStatus = SendPreamble(m_clientSocket);

			// try sending request
			if (returnStatus.ok())
			{
				returnStatus = SendEnvelope(m_clientSocket, envelopeToSend);
			}
		}

		if (returnStatus.ok())
		{
			break;
		}
		else if (retryCounter > 0)
		{
			CloseSocket(m_clientSocket);
			m_clientSocket = InvalidSocket;

			// wait to not spam the socket and start over
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	}
	envelopeToSend.data.reset();

	if (!returnStatus.ok())
	{
		CloseSocket(m_clientSocket);
		m_clientSocket = InvalidSocket;
		return returnStatus;
	}

	// receive response
	{
		Envelope envelope{};

		returnStatus = ReceiveEnvelope(m_clientSocket, envelope);

		if (!returnStatus.ok())
		{
			return returnStatus;
		}

		if (envelope.comId != comId)
		{
			return {
				StatusCode::LOGIC_ERROR,
				"ChannelInterface::Call: invalid comId (expected: '" + comId +
				"', received: '" + envelope.comId + "')"};
		}

		if (envelope.functionId != functionId)
		{
			return {
				StatusCode::LOGIC_ERROR,
				"ChannelInterface::Call: invalid functionId (expected: '" + std::to_string(functionId) +
				"', received: '" + std::to_string(envelope.functionId) + "')"};
		}

		returnStatus = Status{static_cast<StatusCode>(envelope.statusCode), envelope.statusMessage};

		response.swap(envelope.data);
	}

	return returnStatus;
}

Status ChannelInterface::Call(
	const ClientContext& context,
	int64_t functionId,
	const ::google::protobuf::MessageLite& request,
	::google::protobuf::MessageLite& response)
{
	auto comIdFound = context.client_metadata().find(ServiceBase::CommunicationIdToken);
	if (comIdFound == context.client_metadata().end())
	{
		return {
			StatusCode::FAILED_PRECONDITION,
			"ChannelInterface::Call: metadata holds no communication id token"};
	}

	std::shared_ptr<std::string> requestRaw = std::make_shared<std::string>();
	std::shared_ptr<std::string> responseRaw;

	if (!request.SerializeToString(requestRaw.get()))
	{
		return {
			StatusCode::LOGIC_ERROR,
			"ChannelInterface::Call: serializing request failed"};
	}

	Status status = Call(comIdFound->second, functionId, std::move(requestRaw), responseRaw);

	if (status.ok())
	{
		if (!response.ParseFromString(*responseRaw))
		{
			status = {
				StatusCode::LOGIC_ERROR,
				"ChannelInterface::Call: parsing response failed"};
		}
	}

	return status;
}

Status ChannelInterface::Call(
	const std::string& comId,
	int64_t functionId,
	const ::google::protobuf::MessageLite& request,
	::google::protobuf::MessageLite& response)
{
	ClientContext ctx;
	ctx.AddMetadata(ServiceBase::CommunicationIdToken, comId);
	return Call(ctx, functionId, request, response);
}

Status ChannelInterface::Connect()
{
	if (m_clientSocket != InvalidSocket)
	{
		CloseSocket(m_clientSocket);
		m_clientSocket = InvalidSocket;
	}

	sockaddr_in serverAddress{};

	serverAddress.sin_family = AF_INET;

	if (ParseLocationUri(m_location, serverAddress.sin_addr, serverAddress.sin_port))
	{
		// create socket
		m_clientSocket = ::socket(AF_INET, SocketType, 0);
		if (m_clientSocket == InvalidSocket)
		{
			return {
				StatusCode::CONNECT_ERROR,
				"ChannelInterface::Connect: socket() failed; location: " + m_location +
				"; last error " + std::to_string(GetLastSocketError())};
		}

		ResetLastSocketError();
		int connectResult = ::connect(m_clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress));
		if (connectResult < 0)
		{
			const int lastError = GetLastSocketError();
			CloseSocket(m_clientSocket);
			m_clientSocket = InvalidSocket;
			return {
				StatusCode::CONNECT_ERROR,
				"ChannelInterface::Connect: connect() failed; location: " + m_location +
				"; last error " + std::to_string(lastError)};
		}

		timeval timeout{};
		timeout.tv_sec = SendReceiveTimeout;

		if (::setsockopt(m_clientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) != 0 ||
			::setsockopt(m_clientSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) != 0)
		{
			const int lastError = GetLastSocketError();
			CloseSocket(m_clientSocket);
			m_clientSocket = InvalidSocket;
			return {
				StatusCode::CONNECT_ERROR,
				"ChannelInterface::Connect: setsockopt() failed; location: " + m_location +
				"; last error " + std::to_string(lastError)};
		}
	}
	else
	{
		return {
			StatusCode::CONNECT_ERROR,
			"ChannelInterface::Connect: failed to parse location; location: " + m_location};
	}

	return Status::OK;
}

} // namespace SocketIO

} // namespace Transport

} // namespace TVRemoteScreenSDKCommunication
