//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2021 TeamViewer Germany GmbH                                     //
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

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/TransportFramework.h>

#include <TVRemoteScreenSDKCommunication/InstantSupportService/InstantSupportData.h>
#include <TVRemoteScreenSDKCommunication/InstantSupportService/InstantSupportError.h>

namespace TestInstantSupportService
{
template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
struct TestData;

template<>
struct TestData<TVRemoteScreenSDKCommunication::gRPCTransport>
{
	static constexpr const char* Socket = "unix:///tmp/instantSupport_test";
	static constexpr const char* ComId = "tokengRPC";
	static constexpr const char* AccessToken = "TestAccessToken";
	static constexpr const char* Group = "TestGroup";
	static constexpr const char* Description = "TestDescription";
	static constexpr const char* Email = "max.mustermann@example.com";
	static constexpr const char* SessionCode = "TestSessionCode";
	static constexpr const char* Name = "Name";

	static constexpr TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError InvalidToken
		= TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError::InvalidToken;

	static const TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportData& Data()
	{
		static const TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportData data
			{ SessionCode, Name, Description, TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportState::Open };
		return data;
	}
};

template<>
struct TestData<TVRemoteScreenSDKCommunication::TCPSocketTransport>
{
	static constexpr const char* Socket = "tv+tcp://127.0.0.1:9003";
	static constexpr const char* ComId = "tokenSocketIO";
	static constexpr const char* AccessToken = "Test_Access_Token";
	static constexpr const char* Group = "Test_Group";
	static constexpr const char* Description = "Test_Description";
	static constexpr const char* Email = "max.mustermann@example.de";
	static constexpr const char* SessionCode = "Test-Session-Code";
	static constexpr const char* Name = "__Name__";

	static constexpr TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError InvalidToken
		= TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError::InvalidToken;

	static const TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportData& Data()
	{
		static const TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportData data
			{ SessionCode, Name, Description, TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportState::Closed };
		return data;
	}
};
} // namespace TestInstantSupportService
