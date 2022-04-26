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
#include "ViewGeometryServiceServer.h"

namespace TVRemoteScreenSDKCommunication
{
namespace ViewGeometryService
{

ViewGeometryServiceServer::ViewGeometryServiceServer()
	: BaseType{ServiceType::ViewGeometry}
{
}

ViewGeometryServiceServer::~ViewGeometryServiceServer() = default;

void ViewGeometryServiceServer::SetUpdateAreaOfInterestCallback(
	const UpdateAreaOfInterestCallback& requestProcessing)
{
	SetCallback<CBEnum::UpdateAreaOfInterest>(requestProcessing);
}

void ViewGeometryServiceServer::SetUpdateVirtualDesktopCallback(
	const UpdateVirtualDesktopCallback& requestProcessing)
{
	SetCallback<CBEnum::UpdateVirtualDesktop>(requestProcessing);
}

::grpc::Status ViewGeometryServiceServer::UpdateAreaOfInterest(
	::grpc::ServerContext* context,
	const ::tvviewgeometryservice::UpdateAreaOfInterestRequest* request,
	::tvviewgeometryservice::UpdateAreaOfInterestResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::UpdateAreaOfInterest,
		::tvviewgeometryservice::UpdateAreaOfInterestRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			const auto& aoi = request.areaofinterest();
			const Rect areaOfInterest{aoi.x(), aoi.y(), aoi.width(), aoi.height()};

			callback(comId, areaOfInterest, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status ViewGeometryServiceServer::UpdateVirtualDesktop(
	::grpc::ServerContext* context,
	const ::tvviewgeometryservice::UpdateVirtualDesktopRequest* request,
	::tvviewgeometryservice::UpdateVirtualDesktopResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::UpdateVirtualDesktop,
		::tvviewgeometryservice::UpdateVirtualDesktopRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			const auto& geometry = request.virtualdesktopgeometry();

			VirtualDesktop virtualDesktop;
			virtualDesktop.width = geometry.width();
			virtualDesktop.height = geometry.height();
			virtualDesktop.screens.reserve(geometry.screens_size());

			for (int i = 0; i < geometry.screens_size(); ++i)
			{
				const auto& screen = geometry.screens(i);
				const auto& rect = screen.geometry();
				virtualDesktop.screens.push_back(Screen{
					screen.name(),
					Rect{rect.x(), rect.y(), rect.width(), rect.height()}});
			}

			callback(comId, virtualDesktop, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

} // namespace ViewGeometryService
} // namespace TVRemoteScreenSDKCommunication
