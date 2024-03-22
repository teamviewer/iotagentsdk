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
#include "Socket.h"

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/ParseUrl.h>

#if defined(_WIN32) || defined(WINCE)
#ifdef WINCE
#pragma comment(lib, "Ws2.lib")
#else // _WINCE
#pragma comment(lib, "Ws2_32.lib")
#endif // _WINCE
#else // _WIN32 || _WINCE
#include <netdb.h>
#include <unistd.h>
#endif // _WIN32 || _WINCE

#include <array>
#include <algorithm>
#include <mutex>
#include <limits>
#include <thread>

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#undef min
#endif

#include "Envelope.pb.h"

namespace TVRemoteScreenSDKCommunication
{
namespace Transport
{
namespace SocketIO
{

#if defined(_WIN32) || defined(_WINCE)
#define TV_SOCKET_ERROR(error_code) WSA ## error_code
const int MSG_MORE = MSG_PARTIAL;

void ResetLastSocketError()
{
	::WSASetLastError(0);
}

int GetLastSocketError()
{
	return ::WSAGetLastError();
}

int CloseSocket(socket_t socket)
{
	::shutdown(socket, SD_BOTH);
	return ::closesocket(socket);
}

class SocketSetup
{
public:
	SocketSetup()
	{
		WSADATA wsaData;
		m_doCleanup = ::WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
	}

	~SocketSetup()
	{
		if (m_doCleanup)
		{
			::WSACleanup();
		}
	}
private:
	bool m_doCleanup;
};

#else // _WIN32 || _WINCE
#define TV_SOCKET_ERROR(error_code) error_code

struct SocketSetup
{
};

void ResetLastSocketError()
{
	errno = 0;
}

int GetLastSocketError()
{
	return errno;
}

int CloseSocket(socket_t socket)
{
	::shutdown(socket, SHUT_RDWR);
	return ::close(socket);
}

#endif // _WIN32 || _WINCE

bool ParseLocationUri(const std::string& locationUri, in_addr& address, in_port_t& port)
{
	UrlComponents components{};
	if (!ParseUrl(locationUri, components))
	{
		return false;
	}

	if (GetTransportFramework(components.scheme) != TransportFramework::TCPSocketTransport)
	{
		return false;
	}

	addrinfo hints{};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	addrinfo *hostInfo{};
	if (::getaddrinfo(components.host.c_str(), /*service=*/nullptr, &hints, &hostInfo) != 0)
	{
		return false;
	}
	std::unique_ptr<addrinfo, void(*)(addrinfo*)> hostInfoPtr{hostInfo, &::freeaddrinfo};

	if (hostInfo && hostInfo->ai_addr)
	{
		address = reinterpret_cast<sockaddr_in*>(hostInfo->ai_addr)->sin_addr;
	}
	else
	{
		return false;
	}

	port = htons(components.port);

	return true;
}

std::shared_ptr<SocketSetup> QuerySockets()
{
	static std::mutex globalSetupMutex;
	static std::weak_ptr<SocketSetup> globalSetup;

	std::lock_guard<std::mutex> lock(globalSetupMutex);
	std::shared_ptr<SocketSetup> result = globalSetup.lock();
	if (!result)
	{
		result.reset(new SocketSetup);
		globalSetup = result;
	}

	return result;
}


void ReceiveWithRetry(ssize_t& receivedBytes, int& lastError, socket_t socket, char* buffer, size_t bufferSize, int flags)
{
	do
	{
		ResetLastSocketError();
		receivedBytes = ::recv(socket, buffer, bufferSize, flags);
		lastError = GetLastSocketError();
	}
	while (receivedBytes < 0 && lastError == TV_SOCKET_ERROR(EINTR));
}

void SendWithRetry(ssize_t& sentSize, int& lastError, socket_t socket, const char* buffer, size_t bufferSize, int flags)
{
	do
	{
		ResetLastSocketError();
		sentSize = ::send(socket, buffer, bufferSize, flags);
		lastError = GetLastSocketError();
	}
	while (sentSize < 0 && lastError == TV_SOCKET_ERROR(EINTR));
}

Status ReceiveData(socket_t socket, std::string& dataBuffer)
{
	uint32_t dataLength = 0;

	// receive length
	{
		int lastError = 0;
		ssize_t receivedBytes = 0;
		ReceiveWithRetry(receivedBytes, lastError, socket, reinterpret_cast<char*>(&dataLength), sizeof(dataLength), 0);
		dataLength = ::ntohl(dataLength);

		const bool headerIsValid = (receivedBytes == sizeof(dataLength));
		if (!headerIsValid)
		{
			return{
				StatusCode::IO_ERROR,
				"ReceiveData: invalid data length; receivedBytes " + std::to_string(receivedBytes) +
				"; received data length " + std::to_string(dataLength) +
				"; last error " + std::to_string(lastError)};
		}
	}

	if (dataLength == 0)
	{
		dataBuffer.clear();
		return Status::OK;
	}

	dataBuffer.resize(dataLength);

	// read data buffer

	size_t effectiveChunkSize = 0;
	size_t bufferPosition = 0;
	while (bufferPosition < dataLength)
	{
		effectiveChunkSize = std::min(static_cast<size_t>(dataLength) - bufferPosition, ChunkSize);

		int lastError = 0;
		ssize_t receivedSize = 0;
		ReceiveWithRetry(receivedSize, lastError, socket, &dataBuffer.at(bufferPosition), effectiveChunkSize, 0);
		if (receivedSize < 0)
		{
			return {
				StatusCode::IO_ERROR,
				"ReceiveData: recv() failed; last error: " + std::to_string(lastError)};
		}
		else if (receivedSize == 0)
		{
			break;
		}

		bufferPosition += static_cast<size_t>(receivedSize);
	}

	// check expected size
	if (dataBuffer.size() < bufferPosition)
	{
		return {
			StatusCode::IO_ERROR,
			"ReceiveData: buffer size does not match: " + std::to_string(dataBuffer.size()) +
			" < " + std::to_string(dataLength)};
	}

	return Status::OK;
}

Status ReceiveEnvelope(socket_t socket, Envelope& envelope)
{
	// message structure: MAGIC (4 bytes)|data length (4 bytes)|data
	{
		uint32_t magicBuffer = 0;

		int lastError = 0;
		ssize_t receivedBytes = 0;
		ReceiveWithRetry(receivedBytes, lastError, socket, reinterpret_cast<char*>(&magicBuffer), sizeof(magicBuffer), 0);
		magicBuffer = ::ntohl(magicBuffer);

		const bool headerIsValid = (receivedBytes == sizeof(magicBuffer)) && (magicBuffer == MagicNumber);
		if (!headerIsValid)
		{
			return {StatusCode::IO_ERROR,
				"ReceiveEnvelope: invalid header; receivedBytes: " + std::to_string(receivedBytes) +
				"; magic: " + std::to_string(magicBuffer) +
				"; lastError: " + std::to_string(lastError)};
		}
	}

	// read meta data
	std::string metaDataBuffer;

	Status result = ReceiveData(socket, metaDataBuffer);
	if (!result.ok())
	{
		return result;
	}

	if (!envelope.ParseMetaData(metaDataBuffer))
	{
		return {
			StatusCode::LOGIC_ERROR,
			"ReceiveEnvelope: parsing envelope failed"};
	}

	std::shared_ptr<std::string> payload = std::make_shared<std::string>();

	// read payload
	result = ReceiveData(socket, *payload);
	if (!result.ok())
	{
		return result;
	}

	envelope.data = payload;

	return Status::OK;
}

Status SendData(socket_t socket, const std::string& dataBuffer, bool sendMore)
{
	// send data length
	{
		uint32_t dataLengthBuffer = htonl(static_cast<uint32_t>(dataBuffer.size()));
		const bool doSendMore = sendMore || !dataBuffer.empty();
		int lastError = 0;
		ssize_t sentSize = 0;
		SendWithRetry(sentSize, lastError, socket, reinterpret_cast<char*>(&dataLengthBuffer), sizeof(dataLengthBuffer), doSendMore ? MSG_MORE : 0);
		if (sentSize != sizeof(dataLengthBuffer))
		{
			return {
				StatusCode::IO_ERROR,
				"SendData: send() data length failed; last error: " + std::to_string(lastError) +
				", sent bytes: " + std::to_string(sentSize)};
		}
	}

	// send data in chunks
	size_t effectiveChunkSize = 0;
	size_t bufferPosition = 0;

	while (bufferPosition < dataBuffer.size())
	{
		effectiveChunkSize = std::min(dataBuffer.size() - bufferPosition, ChunkSize);

		const bool doSendMore = sendMore || bufferPosition + effectiveChunkSize < dataBuffer.size();
		int lastError = 0;
		ssize_t sentSize = 0;
		SendWithRetry(sentSize, lastError, socket, &dataBuffer.at(bufferPosition), effectiveChunkSize, doSendMore ? MSG_MORE : 0);
		if (sentSize <= 0)
		{
			return {
				StatusCode::IO_ERROR,
				"SendData: send() data failed; last error: " + std::to_string(lastError) +
				"; sent bytes: " + std::to_string(sentSize)};
		}

		bufferPosition += static_cast<size_t>(sentSize);
	}

	return Status::OK;
}

Status SendEnvelope(socket_t socket, const Envelope& envelope)
{
	if (!envelope.data)
	{
		return {StatusCode::LOGIC_ERROR, "no data"};
	}

	// send header
	{
		uint32_t magicNumberBuffer = htonl(MagicNumber);
		int lastError = 0;
		ssize_t sentSize = 0;
		SendWithRetry(sentSize, lastError, socket, reinterpret_cast<char*>(&magicNumberBuffer), sizeof(magicNumberBuffer), MSG_MORE);
		if (sentSize != sizeof(magicNumberBuffer))
		{
			return {StatusCode::IO_ERROR,
				"SendEnvelope: send() failed for header; last error: " + std::to_string(lastError) +
				"; sent bytes: " + std::to_string(sentSize)};
		}
	}

	// send meta data
	std::string metaDataBuffer = envelope.SerializeMetaData();
	Status result = SendData(socket, metaDataBuffer, true);
	if (!result.ok())
	{
		return result;
	}

	// send data in chunks
	return SendData(socket, *(envelope.data), false);
}

Status ReceivePreamble(socket_t socket)
{
	if (socket == InvalidSocket)
	{
		return {StatusCode::IO_ERROR, "SendPreamble: not connected / invalid socket"};
	}

	uint32_t buffer = 0;

	int lastError = 0;
	ssize_t receivedBytes = 0;
	ReceiveWithRetry(receivedBytes, lastError, socket, reinterpret_cast<char*>(&buffer), sizeof(buffer), 0);

	if (receivedBytes == 0)
	{
		return {StatusCode::CONNECTION_CLOSED, "ReceivePreamble: connection closed"};
	}

	if (receivedBytes < 0)
	{
		return {
			StatusCode::IO_ERROR,
			"ReceivePreamble: recv() failed; last error: " + std::to_string(lastError)};
	}

	uint32_t magicBuffer = ::ntohl(buffer);
	const bool isValid = (receivedBytes == sizeof(buffer)) && (magicBuffer == MagicPreamble);
	if (!isValid)
	{
		return {
			StatusCode::IO_ERROR,
			"ReceivePreamble: invalid preamble; received bytes " + std::to_string(receivedBytes) +
			"; received magic " + std::to_string(magicBuffer)};
	}

	return Status::OK;
}

Status SendPreamble(socket_t socket)
{
	if (socket == InvalidSocket)
	{
		return {
			StatusCode::IO_ERROR,
			"SendPreamble: not connected / invalid socket"};
	}

	uint32_t buffer = ::htonl(MagicPreamble);

	int lastError = 0;
	ssize_t sentSize = 0;
	SendWithRetry(sentSize, lastError, socket, reinterpret_cast<char*>(&buffer), sizeof(buffer), MSG_MORE);
	if (sentSize < static_cast<ssize_t>(sizeof(buffer)))
	{
		return {
			StatusCode::IO_ERROR,
			"SendPreamble: send() failed; sent bytes: " + std::to_string(sentSize) +
			"; last error: " + std::to_string(lastError)};
	}

	return Status::OK;
}

std::string Envelope::SerializeMetaData() const
{
	tvsocketservicebase::Envelope intermediate{};
	intermediate.set_com_id(comId);
	intermediate.set_function_id(functionId);
	intermediate.set_status_code(statusCode);
	intermediate.set_status_message(statusMessage);

	std::string result = intermediate.SerializeAsString();
	return result;
}

bool Envelope::ParseMetaData(const std::string& metadata)
{
	tvsocketservicebase::Envelope intermediate;
	if (!intermediate.ParseFromString(metadata))
	{
		return false;
	}

	comId = intermediate.com_id();
	functionId = intermediate.function_id();
	statusCode = intermediate.status_code();
	statusMessage = intermediate.status_message();

	return true;
}

} // namespace SocketIO
} // namespace Transport
} // namespace TVRemoteScreenSDKCommunication
