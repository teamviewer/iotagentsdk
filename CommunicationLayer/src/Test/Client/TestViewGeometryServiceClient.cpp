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
#include "TestViewGeometryServiceClient.h"

#include "TestData/TestDataViewGeometry.h"

#include <TVRemoteScreenSDKCommunication/ViewGeometryService/IViewGeometryServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ViewGeometryService/ServiceFactory.h>

#include <iostream>

namespace TestViewGeometryService
{

constexpr const char* LogPrefix = "[ViewGeometryService][Client] ";

int TestViewGeometryServiceClient(int /*argc*/, char** /*argv*/)
{
	using namespace TVRemoteScreenSDKCommunication::ViewGeometryService;

	const auto client = ServiceFactory::CreateClient();

	if (client->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::ViewGeometry)
	{
		std::cerr << LogPrefix << "Unexpected service type\n";
		exit(EXIT_FAILURE);
	}

	client->StartClient(TestData::Socket);
	if (client->GetDestination() != TestData::Socket)
	{
		std::cerr << LogPrefix << "Unexpected location\n";
		exit(EXIT_FAILURE);
	}

	auto result = client->UpdateVirtualDesktop(TestData::ComId, TestData::virtualDesktop);
	if (result.IsOk())
	{
		std::cout << LogPrefix << "UpdateVirtualDesktop successful\n";
	}
	else
	{
		std::cerr << LogPrefix << "UpdateVirtualDesktop Error: " <<
			result.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	result = client->UpdateAreaOfInterest(TestData::ComId, TestData::areaOfInterest);
	if (result.IsOk())
	{
		std::cout << LogPrefix << "UpdateAreaOfInterest successful\n";
	}
	else
	{
		std::cerr << LogPrefix << "UpdateAreaOfInterest Error: " <<
			result.errorMessage << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

} // namespace TestViewGeometryService