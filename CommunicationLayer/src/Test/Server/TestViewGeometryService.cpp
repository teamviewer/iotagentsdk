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
#include "TestViewGeometryService.h"

#include "TestData/TestDataViewGeometry.h"

#include <TVRemoteScreenSDKCommunication/ViewGeometryService/ServiceFactory.h>

#include <algorithm>
#include <cstring>
#include <iostream>

namespace TestViewGeometryService
{

constexpr const char* LogPrefix = "[ViewGeometryService][Server] ";

std::shared_ptr<TVRemoteScreenSDKCommunication::ViewGeometryService::IViewGeometryServiceServer>
TestViewGeometryService()
{
	using namespace TVRemoteScreenSDKCommunication::ViewGeometryService;
	std::shared_ptr<IViewGeometryServiceServer> server = ServiceFactory::CreateServer();
	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::ViewGeometry)
	{
		std::cerr << LogPrefix << "Unexpected service type\n";
		exit(EXIT_FAILURE);
	}

	const auto updateVirtualDesktop = [](
		const std::string& comId,
		const VirtualDesktop& virtualDesktop,
		const IViewGeometryServiceServer::UpdateVirtualDesktopResponseCallback& response)
	{
		std::cout << LogPrefix << "Received UpdateVirtualDesktop with: " << comId << std::endl;
		const bool test1 = virtualDesktop.width == TestData::virtualDesktop.width,
			test2 = virtualDesktop.height == TestData::virtualDesktop.height,
			test3 = std::equal(
				std::begin(virtualDesktop.screens),
				std::end(virtualDesktop.screens),
				std::begin(TestData::virtualDesktop.screens),
				std::end(TestData::virtualDesktop.screens),
				[](const Screen& a, const Screen& b)
				{
					return a.name == b.name &&
						!std::memcmp(&a.geometry, &b.geometry, sizeof(Rect));
				});


		if (comId == TestData::ComId && test1 && test2 && test3)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetUpdateVirtualDesktopCallback(updateVirtualDesktop);

	const auto updateAreaOfInterest = [](
		const std::string& comId,
		const Rect& areaOfInterest,
		const IViewGeometryServiceServer::UpdateAreaOfInterestResponseCallback& response)
	{
		std::cout << LogPrefix << "Received UpdateVirtualDesktop with: " << comId << std::endl;
		const bool test =
			!std::memcmp(&areaOfInterest, &TestData::areaOfInterest, sizeof(Rect));
		if (comId == TestData::ComId && test)
		{
			response(TVRemoteScreenSDKCommunication::CallState::Ok);
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

} // namespace TestViewGeometryService