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

#include "ColorFormat.h"

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/CallStatus.h>
#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/IServiceServer.h>

#include <string>
#include <functional>
#include <vector>

namespace TVRemoteScreenSDKCommunication
{
namespace ImageService
{

class IImageServiceServer : public IServiceServer
{
public:
	virtual ~IImageServiceServer() = default;

	using ImageUpdateResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessImageUpdateRequestCallback = std::function<void(
		const std::string& comId,
		int32_t x,
		int32_t y,
		int32_t width,
		int32_t height,
		const std::string& pictureData,
		const ImageUpdateResponseCallback& response)>;

	virtual void SetImageUpdateCallback(const ProcessImageUpdateRequestCallback& requestProcessing) = 0;

	using ImageDefinitionResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessImageDefinitionRequestCallback = std::function<void(
		const std::string& comId,
		const std::string& imageSourceTitle,
		int32_t width,
		int32_t height,
		ColorFormat format,
		double dpi,
		const ImageDefinitionResponseCallback& response)>;

	virtual void SetImageDefinitionCallback(const ProcessImageDefinitionRequestCallback& requestProcessing) = 0;
};

} // namespace ImageService
} // namespace TVRemoteScreenSDKCommunication
