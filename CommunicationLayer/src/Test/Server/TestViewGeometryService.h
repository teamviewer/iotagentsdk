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

#include "TestData/TestDataViewGeometry.h"

#include <TVRemoteScreenSDKCommunication/ViewGeometryService/IViewGeometryServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ViewGeometryService/ServiceFactory.h>

#include <algorithm>
#include <cstring>
#include <iostream>

namespace TestViewGeometryService
{

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
std::shared_ptr<TVRemoteScreenSDKCommunication::ViewGeometryService::IViewGeometryServiceServer>
TestViewGeometryService()
{
	using namespace TVRemoteScreenSDKCommunication::ViewGeometryService;
	using TVRemoteScreenSDKCommunication::CallStatus;
	using TVRemoteScreenSDKCommunication::CallState;
	using TestData = TestData<Framework>;
	const std::string LogPrefix = "[ViewGeometryService][Server][fw=" + std::to_string(Framework) + "] ";

	std::shared_ptr<IViewGeometryServiceServer> server = ServiceFactory::CreateServer<Framework>();
	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::ViewGeometry)
	{
		std::cerr << LogPrefix << "Unexpected service type\n";
		exit(EXIT_FAILURE);
	}

	const auto updateVirtualDesktop = [LogPrefix](
		const std::string& comId,
		const VirtualDesktop& virtualDesktop,
		const IViewGeometryServiceServer::UpdateVirtualDesktopResponseCallback& response)
	{
		std::cout << LogPrefix << "Received UpdateVirtualDesktop with: " << comId << std::endl;
		const bool test1 = virtualDesktop.width == TestData::virtualDesktop().width,
			test2 = virtualDesktop.height == TestData::virtualDesktop().height,
			test3 = [virtualDesktop]()
			{
				if (virtualDesktop.screens.size() != TestData::virtualDesktop().screens.size())
				{
					return false;
				}

				for (auto aIt = std::begin(virtualDesktop.screens), bIt = std::begin(TestData::virtualDesktop().screens);
					aIt != std::end(virtualDesktop.screens) && bIt != std::end(TestData::virtualDesktop().screens);
					++aIt, ++bIt)
				{
					if (aIt->name != bIt->name || std::memcmp(&aIt->geometry, &bIt->geometry, sizeof(Rect)))
					{
						return false;
					}
				}
				return true;
			}();


		if (comId == TestData::ComId && test1 && test2 && test3)
		{
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetUpdateVirtualDesktopCallback(updateVirtualDesktop);

	const auto updateAreaOfInterest = [LogPrefix](
		const std::string& comId,
		const Rect& areaOfInterest,
		const IViewGeometryServiceServer::UpdateAreaOfInterestResponseCallback& response)
	{
		std::cout << LogPrefix << "Received UpdateVirtualDesktop with: " << comId << std::endl;
		const bool test =
			!std::memcmp(&areaOfInterest, &TestData::areaOfInterest(), sizeof(Rect));
		if (comId == TestData::ComId && test)
		{
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetUpdateAreaOfInterestCallback(updateAreaOfInterest);

	server->StartServer(TestData::Socket);

	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}

} // namespace TestSessionStatusService
