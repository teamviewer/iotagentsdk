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
// ==================================
// Generated by TVCMGen. DO NOT EDIT!
// ==================================

#include "ImageServiceSocketIOClient.h"

#include <TVRemoteScreenSDKCommunication/ServiceBase/ClientErrorMessage.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/RequestMetadata.h>

#include "internal/ImageServiceFunctions.h"

#include "ColorFormat.pb.h"
#include "GrabResult.pb.h"
#include "ImageDefinitionRequest.pb.h"
#include "ImageDefinitionResponse.pb.h"
#include "ImageUpdateResponse.pb.h"

namespace TVRemoteScreenSDKCommunication
{

namespace ImageService
{

void ImageServiceSocketIOClient::StartClient(const std::string& destination)
{
	m_destination = destination;

	m_channel.reset(new Transport::SocketIO::ChannelInterface(m_destination));
}

void ImageServiceSocketIOClient::StopClient(bool /*force*/)
{
	m_channel.reset();
}

ServiceType ImageServiceSocketIOClient::GetServiceType() const
{
	return ServiceType::Image;
}

const std::string& ImageServiceSocketIOClient::GetDestination() const
{
	return m_destination;
}

// rpc call UpdateImage
auto ImageServiceSocketIOClient::UpdateImage(const std::string& comId,
	int32_t x,
	int32_t y,
	int32_t width,
	int32_t height,
	const std::string& pictureData) -> CallStatus
{
	CallStatus returnValue{};

	if (m_channel == nullptr)
	{
		returnValue.errorMessage = TvServiceBase::ErrorMessage_MissingStartClient;
		return returnValue;
	}

	if (comId.empty())
	{
		returnValue.errorMessage = TvServiceBase::ErrorMessage_InvalidInputParameter;
		return returnValue;
	}

	::tvimageservice::GrabResult request{};

	request.set_chunks(1);
	::tvimageservice::Rect* dirtyRect = request.mutable_dirtyrect();
	dirtyRect->set_x(x);
	dirtyRect->set_y(y);
	dirtyRect->set_height(height);
	dirtyRect->set_width(width);
	request.mutable_pixeldata()->set_picture(pictureData);

	::tvimageservice::ImageUpdateResponse response{};

	Transport::SocketIO::Status status = m_channel->Call(comId, Function_UpdateImage, request, response);

	if (status.ok())
	{
		returnValue = CallStatus{CallState::Ok};
	}
	else
	{
		returnValue.errorMessage = status.error_message();
	}

	return returnValue;
}

// rpc call UpdateImageDefinition
auto ImageServiceSocketIOClient::UpdateImageDefinition(const std::string& comId,
	const std::string& imageSourceTitle,
	int32_t width,
	int32_t height,
	ColorFormat format,
	double dpi) -> CallStatus
{
	CallStatus returnValue{};

	if (m_channel == nullptr)
	{
		returnValue.errorMessage = TvServiceBase::ErrorMessage_MissingStartClient;
		return returnValue;
	}

	if (comId.empty())
	{
		returnValue.errorMessage = TvServiceBase::ErrorMessage_InvalidInputParameter;
		return returnValue;
	}

	::tvimageservice::ImageDefinitionRequest request{};

	request.set_imagesourcetitle(imageSourceTitle);

	request.set_width(width);

	request.set_height(height);

	::tvimageservice::ColorFormat formatProtoValue = ::tvimageservice::ColorFormat::Unknown;

	switch (format)
	{
		case ColorFormat::Unknown:
			formatProtoValue = ::tvimageservice::ColorFormat::Unknown;
			break;
		case ColorFormat::BGRA32:
			formatProtoValue = ::tvimageservice::ColorFormat::BGRA32;
			break;
		case ColorFormat::RGBA32:
			formatProtoValue = ::tvimageservice::ColorFormat::RGBA32;
			break;
		case ColorFormat::R5G6B5:
			formatProtoValue = ::tvimageservice::ColorFormat::R5G6B5;
			break;
		default:

			break;
	}

	request.set_format(formatProtoValue);

	request.set_dpi(dpi);

	::tvimageservice::ImageDefinitionResponse response{};

	Transport::SocketIO::Status status = m_channel->Call(comId, Function_UpdateImageDefinition, request, response);

	if (status.ok())
	{
		returnValue = CallStatus{CallState::Ok};
	}
	else
	{
		returnValue.errorMessage = status.error_message();
	}

	return returnValue;
}

} // namespace ImageService

} // namespace TVRemoteScreenSDKCommunication