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
#include "InputServiceClient.h"

#include <TVRemoteScreenSDKCommunication/ServiceBase/ClientErrorMessage.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/RequestMetadata.h>

#include <grpcpp/create_channel.h>

namespace TVRemoteScreenSDKCommunication
{
namespace InputService
{

InputServiceClient::InputServiceClient()
	: BaseType{ServiceType::Input}
{
}

CallStatus InputServiceClient::SimulateKey(
	const std::string& comId,
	KeyState keyState,
	uint32_t xkbSymbol,
	uint32_t unicodeCharacter,
	uint32_t xkbModifiers)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvinputservice::KeyRequest;
		using Response = ::tvinputservice::KeyResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			KeyState keyState,
			uint32_t xkbSymbol,
			uint32_t unicodeCharacter,
			uint32_t xkbModifiers)
		{
			const ::tvinputservice::KeyRequest_KeyState wireFormatKeyState =
				keyState == KeyState::Down ?
				::tvinputservice::KeyRequest_KeyState_Down :
				::tvinputservice::KeyRequest_KeyState_Up;

			request.set_keystate(wireFormatKeyState);
			request.set_unicodecharacter(unicodeCharacter);
			request.set_xkbsymbol(xkbSymbol);
			request.set_xkbmodifiers(xkbModifiers);

			return stub.SimulateKey(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, keyState, xkbSymbol, unicodeCharacter, xkbModifiers);
}

CallStatus InputServiceClient::SimulateMouseMove(
	const std::string& comId,
	int32_t posX,
	int32_t posY)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvinputservice::MouseMoveRequest;
		using Response = ::tvinputservice::MouseMoveResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			int32_t posX,
			int32_t posY)
		{
			request.set_posx(posX);
			request.set_posy(posY);

			return stub.SimulateMouseMove(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, posX, posY);
}

CallStatus InputServiceClient::SimulateMousePressRelease(
	const std::string& comId,
	KeyState keyState,
	int32_t posX,
	int32_t posY,
	MouseButton button)
{
	::tvinputservice::MouseButton mouseButton = ::tvinputservice::MouseButton::Unknown;

	switch (button)
	{
		case MouseButton::Left:
			mouseButton = ::tvinputservice::MouseButton::Left;
			break;
		case MouseButton::Middle:
			mouseButton = ::tvinputservice::MouseButton::Middle;
			break;
		case MouseButton::Right:
			mouseButton = ::tvinputservice::MouseButton::Right;
			break;
		case MouseButton::Unknown:
			break;
	}

	struct Meta: DefaultMeta
	{
		using Request = ::tvinputservice::MousePressReleaseRequest;
		using Response = ::tvinputservice::MousePressReleaseResponse;

		static bool ValidateInput(
			const std::string& comId,
			KeyState /*keyState*/,
			int32_t /*posX*/,
			int32_t /*posY*/,
			::tvinputservice::MouseButton button)
		{
			return DefaultMeta::ValidateInput(comId) &&
				button != ::tvinputservice::MouseButton::Unknown;
		}

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			KeyState keyState,
			int32_t posX,
			int32_t posY,
			::tvinputservice::MouseButton mouseButton)
		{
			const auto wireFormatKeyState =
				keyState == KeyState::Down ?
				::tvinputservice::MousePressReleaseRequest_MouseButtonState::MousePressReleaseRequest_MouseButtonState_Pressed :
				::tvinputservice::MousePressReleaseRequest_MouseButtonState::MousePressReleaseRequest_MouseButtonState_Released;

			request.set_mousebuttonstate(wireFormatKeyState);
			request.set_posx(posX);
			request.set_posy(posY);

			request.set_button(mouseButton);

			return stub.SimulateMousePressRelease(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, keyState, posX, posY, mouseButton);
}

CallStatus InputServiceClient::SimulateMouseWheel(
	const std::string& comId,
	int32_t posX,
	int32_t posY,
	int32_t angle)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvinputservice::MouseWheelRequest;
		using Response = ::tvinputservice::MouseWheelResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			int32_t posX,
			int32_t posY,
			int32_t angle)
		{
			request.set_posx(posX);
			request.set_posy(posY);
			request.set_angle(angle);

			return stub.SimulateMouseWheel(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, posX, posY, angle);
}

} // namespace InputService
} // namespace TVRemoteScreenSDKCommunication
