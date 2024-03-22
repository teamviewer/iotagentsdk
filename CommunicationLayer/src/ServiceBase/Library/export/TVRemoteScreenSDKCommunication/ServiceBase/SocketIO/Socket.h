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

#include "Status.h"

#if defined(_WIN32) || defined(_WINCE)
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <string>
#include <stdint.h>
#include <stddef.h>
#include <memory>

namespace TVRemoteScreenSDKCommunication
{
namespace Transport
{
namespace SocketIO
{

#if defined(_WIN32) || defined(_WINCE)
typedef SOCKET socket_t;
typedef u_long in_addr_t;
typedef u_short in_port_t;
constexpr socket_t InvalidSocket = INVALID_SOCKET;
constexpr int SocketType = SOCK_STREAM;
#else
typedef int socket_t;
constexpr socket_t InvalidSocket = -1;
constexpr int SocketType = SOCK_STREAM | SOCK_CLOEXEC;
#endif

constexpr uint32_t MagicNumber = 0xFF545600;
constexpr uint32_t MagicPreamble = 0xFF005456;
constexpr size_t ChunkSize = 1024 * 1024 * 16; // in bytes;
constexpr uint32_t SendReceiveTimeout = 1; //seconds
constexpr int MaxBacklogSize = 32;

bool ParseLocationUri(const std::string& locationUri, in_addr& address, in_port_t& port);

void ResetLastSocketError();
int GetLastSocketError();

int CloseSocket(socket_t socket);

std::shared_ptr<struct SocketSetup> QuerySockets();

struct Envelope final
{
	Envelope() = default;

	std::string SerializeMetaData() const;
	bool ParseMetaData(const std::string& data);

	std::string comId;
	std::string statusMessage;
	std::shared_ptr<std::string> data;
	int64_t functionId = -1;
	uint32_t statusCode = 0;
};

Status ReceivePreamble(socket_t socket);
Status SendPreamble(socket_t socket);

Status ReceiveEnvelope(socket_t socket, Envelope& envelope);
Status SendEnvelope(socket_t socket, const Envelope& envelope);

} // namespace SocketIO
} // namespace Transport
} // TVRemoteScreenSDKCommunication
