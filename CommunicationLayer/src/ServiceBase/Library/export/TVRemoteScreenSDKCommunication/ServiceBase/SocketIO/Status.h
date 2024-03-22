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

#include <string>
#include <cstdint>

namespace TVRemoteScreenSDKCommunication
{

namespace Transport
{

namespace SocketIO
{

enum class StatusCode : uint32_t
{
	// The names must match the ::grpc::StatusCode names we use, otherwise compile error occurs.
	OK,
	INTERNAL,
	UNAVAILABLE,
	FAILED_PRECONDITION,
	CANCELLED,
	ABORTED,

	CONNECT_ERROR,
	IO_ERROR,
	LOGIC_ERROR,
	CONNECTION_CLOSED,
};

class Status final
{
public:
	Status(StatusCode code, std::string errorMessage);

	bool ok() const { return m_code == StatusCode::OK; };

	StatusCode code() const {return m_code;}
	const std::string& error_message() const {return m_errorMessage;}

	static Status OK;

private:
	StatusCode m_code;
	std::string m_errorMessage;
};

} // namespace SocketIO

} // namespace Transport

} // namespace TVRemoteScreenSDKCommunication
