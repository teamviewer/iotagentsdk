//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2024 TeamViewer Germany GmbH                                     //
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
#include "ParseUrl.h"

#include <cassert>
#include <cstdlib>
#include <limits>
#include <regex>

namespace TVRemoteScreenSDKCommunication
{

namespace
{
const std::regex reURL{ R"(^([a-zA-Z][a-zA-Z0-9+-.]*):\/\/(([\w.]+)(:[1-9]\d*)?)?(\/([a-zA-Z0-9_.-]([a-zA-Z0-9_.-]+\/?)*)?)?$)" };
}

bool ParseUrl(const std::string& url, UrlComponents& urlComponents)
{
	std::smatch match;

	if (std::regex_match(url, match, reURL))
	{
		assert(match.size() == 8);
		long port = 0;
		if (match[4].matched)
		{
			port = strtol(match[4].str().c_str() /*skip :*/ + 1, /*endptr=*/nullptr, 10);
		}
		if (port <= std::numeric_limits<uint16_t>::max())
		{
			std::string scheme = match[1];
			std::transform(scheme.begin(), scheme.end(), scheme.begin(),
				[](unsigned char c) { return std::tolower(c); });
			urlComponents.scheme = std::move(scheme);
			urlComponents.host = match[3];
			urlComponents.port = static_cast<uint16_t>(port);
			urlComponents.path = match[5];
			return true;
		}
	}

	return false;
}

TransportFramework GetTransportFramework(const std::string& scheme)
{
	if (scheme == "unix")
	{
		return TransportFramework::gRPCTransport;
	}

	if (scheme == "tcp+tv" || scheme == "tv+tcp")
	{
		return TransportFramework::TCPSocketTransport;
	}

	return TransportFramework::UnknownTransport;
}

} // namespace TVRemoteScreenSDKCommunication
