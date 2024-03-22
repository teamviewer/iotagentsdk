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

#include <TVRemoteScreenSDKCommunication/ViewGeometryService/VirtualDesktop.h>

namespace TestViewGeometryService
{
template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
struct TestData;

template<>
struct TestData<TVRemoteScreenSDKCommunication::gRPCTransport>
{
	static constexpr const char* Socket = "unix:///tmp/viewGeometry";
	static constexpr const char* ComId = "tokengRPC";

	static const TVRemoteScreenSDKCommunication::ViewGeometryService::VirtualDesktop& virtualDesktop()
	{
		static TVRemoteScreenSDKCommunication::ViewGeometryService::VirtualDesktop vd
			{
				2704,
				1050,
				std::vector<TVRemoteScreenSDKCommunication::ViewGeometryService::Screen>
				{
					{
						"VGA-1",
						TVRemoteScreenSDKCommunication::ViewGeometryService::Rect{0, 0, 1680, 1050}
					},
					{
						"VGA-2",
						TVRemoteScreenSDKCommunication::ViewGeometryService::Rect{1680, 0, 1024, 768}
					}
				}
			};
		return vd;
	}

	static const TVRemoteScreenSDKCommunication::ViewGeometryService::Rect& areaOfInterest()
	{
		static TVRemoteScreenSDKCommunication::ViewGeometryService::Rect aoi
			{
				1680, 0, 1024, 768
			};
		return aoi;
	}
};

template<>
struct TestData<TVRemoteScreenSDKCommunication::TCPSocketTransport>
{
	static constexpr const char* Socket = "tv+tcp://127.0.0.1:9003";
	static constexpr const char* ComId = "tokenSocketIO";

	static const TVRemoteScreenSDKCommunication::ViewGeometryService::VirtualDesktop& virtualDesktop()
	{
		static TVRemoteScreenSDKCommunication::ViewGeometryService::VirtualDesktop vd
			{
				2704,
				1050,
				std::vector<TVRemoteScreenSDKCommunication::ViewGeometryService::Screen>
					{
						{
							"VGA-1",
							TVRemoteScreenSDKCommunication::ViewGeometryService::Rect{0, 0, 1680, 1050}
						},
						{
							"VGA-2",
							TVRemoteScreenSDKCommunication::ViewGeometryService::Rect{1680, 0, 1024, 768}
						}
					}
			};
		return vd;
	}

	static const TVRemoteScreenSDKCommunication::ViewGeometryService::Rect& areaOfInterest()
	{
		static TVRemoteScreenSDKCommunication::ViewGeometryService::Rect aoi
			{
				1680, 0, 1024, 768
			};
		return aoi;
	}
};
} // namespace TestViewGeometryService