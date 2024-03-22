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

#include <TVRemoteScreenSDKCommunication/ImageService/ColorFormat.h>

#include <cstdint>
#include <string>

namespace TestImageService
{
template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
struct TestData;

template<>
struct TestData<TVRemoteScreenSDKCommunication::gRPCTransport>
{
	static constexpr const char* Socket = "unix:///tmp/imageServer";
	static constexpr const char* ComId = "tokengRPC";
	static constexpr const char* ImageSourceTitle = "testImage";
	static constexpr int32_t Width = 12;
	static constexpr int32_t Height = 32;
	static constexpr int32_t X = 0;
	static constexpr int32_t Y = 0;
	static constexpr double Dpi = 60.12;
	static const TVRemoteScreenSDKCommunication::ImageService::ColorFormat ColorFormat =
		TVRemoteScreenSDKCommunication::ImageService::ColorFormat::BGRA32;
	static const std::string& Picture()
	{
		static const std::string pic(8294538, '*');
		return pic;
	}
};

template<>
struct TestData<TVRemoteScreenSDKCommunication::TCPSocketTransport>
{
	static constexpr const char* Socket = "tv+tcp://127.0.0.1:9004";
	static constexpr const char* ComId = "tokenSocketIO";
	static constexpr const char* ImageSourceTitle = "TestImage";
	static constexpr int32_t Width = 12;
	static constexpr int32_t Height = 32;
	static constexpr int32_t X = 0;
	static constexpr int32_t Y = 0;
	static constexpr double Dpi = 60.12;
	static const TVRemoteScreenSDKCommunication::ImageService::ColorFormat ColorFormat =
		TVRemoteScreenSDKCommunication::ImageService::ColorFormat::RGBA32;
	static const std::string& Picture()
	{
		static const std::string pic(8294538, '+');
		return pic;
	}
};
} // namespace TestImageService
