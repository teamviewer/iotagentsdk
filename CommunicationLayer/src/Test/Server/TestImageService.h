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

#include "TestData/TestDataImage.h"

#include <TVRemoteScreenSDKCommunication/ImageService/IImageServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ImageService/ServiceFactory.h>

#include <memory>

namespace TestImageService
{

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
std::shared_ptr<TVRemoteScreenSDKCommunication::ImageService::IImageServiceServer> TestImageService()
{
	using TVRemoteScreenSDKCommunication::CallState;
	using TVRemoteScreenSDKCommunication::CallStatus;
	using namespace TVRemoteScreenSDKCommunication::ImageService;
	using TestData = TestData<Framework>;
	const std::string LogPrefix = "[ImageService][Server][fw=" + std::to_string(Framework) + "] ";

	const std::shared_ptr<IImageServiceServer> server = ServiceFactory::CreateServer<Framework>();
	if (server->GetServiceType() != TVRemoteScreenSDKCommunication::ServiceType::Image)
	{
		std::cerr << LogPrefix << "Unexpected service type" << std::endl;
		exit(EXIT_FAILURE);
	}

	const auto processAnnounceFunction = [LogPrefix](
		const std::string& comId,
		const std::string& imageSourceTitle,
		int32_t width,
		int32_t height,
		ColorFormat format,
		double dpi,
		const IImageServiceServer::UpdateImageDefinitionResponseCallback& response)
	{
		std::cout
			<< LogPrefix
			<< "Received Announce with: "
			<< comId << "(comId), "
			<< imageSourceTitle
			<< "(title), "
			<< width << "(w), " << height << "(h), ";
		switch (format)
		{
		case ColorFormat::BGRA32:
			std::cout << "BGRA32";
			break;
		case ColorFormat::R5G6B5:
			std::cout << "R5G6B5";
			break;
		case ColorFormat::RGBA32:
			std::cout << "RGBA32";
			break;
		case ColorFormat::Unknown:
			std::cout << "Unknown";
			break;
		}
		std::cout << std::endl;

		if (comId == TestData::ComId
			&& imageSourceTitle == TestData::ImageSourceTitle
			&& width == TestData::Width
			&& height == TestData::Height
			&& format == TestData::ColorFormat
			&& dpi == TestData::Dpi)
		{
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr  << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetUpdateImageDefinitionCallback(processAnnounceFunction);

	const auto processImageUpdate = [LogPrefix](
		const std::string& comId,
		int32_t x,
		int32_t y,
		int32_t width,
		int32_t height,
		const std::string& pictureData,
		const IImageServiceServer::UpdateImageDefinitionResponseCallback& response)
	{
		std::cout
			<< LogPrefix
			<< "Received ImageUpdate with: "
			<< comId << "(comId), "
			<< x << "(x), "
			<< y << "(y), "
			<< width << "(w), "
			<< height << "(h), "
			<< pictureData.size()
			<< "(pic bytes)"
			<< std::endl;

		if (comId == TestData::ComId && x == TestData::X && y ==TestData::Y && width == TestData::Width && height == TestData::Height && pictureData == TestData::Picture())
		{
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	server->SetUpdateImageCallback(processImageUpdate);

	const auto processImageChanged = [LogPrefix](
		const std::string& comId,
		const std::string& imageSourceTitle,
		int32_t width,
		int32_t height,
		ColorFormat format,
		double dpi,
		const IImageServiceServer::UpdateImageDefinitionResponseCallback& response)
	{
		std::cout
			<< LogPrefix
			<< "Received ImageChanged with: "
			<< comId << "(comId), "
			<< imageSourceTitle
			<< "(title), "
			<< width
			<< "(w), "
			<< height << "(h), ";

		switch (format)
		{
		case ColorFormat::BGRA32:
			std::cout << "BGRA32";
			break;
		case ColorFormat::R5G6B5:
			std::cout << "R5G6B5";
			break;
		case ColorFormat::RGBA32:
			std::cout << "RGBA32";
			break;
		case ColorFormat::Unknown:
			std::cout << "Unknown";
			break;
		}
		std::cout << std::endl;

		if (comId == TestData::ComId
			&& imageSourceTitle == TestData::ImageSourceTitle
			&& width == TestData::Width
			&& height == TestData::Height
			&& format == TestData::ColorFormat
			&& dpi == TestData::Dpi)
		{
			response(CallStatus::Ok);
		}
		else
		{
			std::cerr  << LogPrefix << "Corrupted Data" << std::endl;
			exit(EXIT_FAILURE);
		}
	};

	server->SetUpdateImageDefinitionCallback(processImageChanged);

	server->StartServer(TestData::Socket);
	if (server->GetLocation() != TestData::Socket)
	{
		std::cerr  << LogPrefix << "Unexpected location" << std::endl;
		exit(EXIT_FAILURE);
	}

	return server;
}

} // namespace TestImageService
