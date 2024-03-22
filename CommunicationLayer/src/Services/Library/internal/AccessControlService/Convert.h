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

#include "AccessControl.pb.h"

#include <TVRemoteScreenSDKCommunication/AccessControlService/AccessControl.h>

namespace TVRemoteScreenSDKCommunication
{
namespace AccessControlService
{

inline bool ValidateAccessControl(::tvaccesscontrolservice::AccessControl feature)
{
	return
		feature == ::tvaccesscontrolservice::AccessControl::FileTransfer ||
		feature == ::tvaccesscontrolservice::AccessControl::RemoteView ||
		feature == ::tvaccesscontrolservice::AccessControl::RemoteControl ||
		feature == ::tvaccesscontrolservice::AccessControl::ScreenRecording;
}

inline bool ToAccessControl(
	::tvaccesscontrolservice::AccessControl fromValue,
	AccessControl& toValue)
{
	using FromType = ::tvaccesscontrolservice::AccessControl;
	using ToType = AccessControl;

	switch (fromValue)
	{
		case FromType::ScreenRecording:
			toValue = ToType::ScreenRecording;
			return true;
		case FromType::RemoteView:
			toValue = ToType::RemoteView;
			return true;
		case FromType::RemoteControl:
			toValue = ToType::RemoteControl;
			return true;
		case FromType::FileTransfer:
			toValue = ToType::FileTransfer;
			return true;
		default:
			break;
	}

	return false;
}

inline bool FromAccessControl(
	AccessControl fromValue,
	::tvaccesscontrolservice::AccessControl& toValue)
{
	using FromType = AccessControl;
	using ToType = ::tvaccesscontrolservice::AccessControl;

	switch (fromValue)
	{
		case FromType::ScreenRecording:
			toValue = ToType::ScreenRecording;
			return true;
		case FromType::RemoteView:
			toValue = ToType::RemoteView;
			return true;
		case FromType::RemoteControl:
			toValue = ToType::RemoteControl;
			return true;
		case FromType::FileTransfer:
			toValue = ToType::FileTransfer;
			return true;
	}

	return false;
}

inline bool ValidateAccess(::tvaccesscontrolservice::Access access)
{
	return
		access == ::tvaccesscontrolservice::Access::Allowed ||
		access == ::tvaccesscontrolservice::Access::AfterConfirmation ||
		access == ::tvaccesscontrolservice::Access::Denied;
}

inline bool ToAccess(
	::tvaccesscontrolservice::Access fromValue,
	Access& toValue)
{
	using FromType = ::tvaccesscontrolservice::Access;
	using ToType = Access;

	switch (fromValue)
	{
		case FromType::Allowed:
			toValue = ToType::Allowed;
			return true;
		case FromType::AfterConfirmation:
			toValue = ToType::AfterConfirmation;
			return true;
		case FromType::Denied:
			toValue = ToType::Denied;
			return true;
		default:
			break;
	}

	return false;
}

inline bool FromAccess(
	Access fromValue,
	::tvaccesscontrolservice::Access& toValue)
{
	using FromType = Access;
	using ToType = ::tvaccesscontrolservice::Access;

	switch (fromValue)
	{
		case FromType::Allowed:
			toValue = ToType::Allowed;
			return true;
		case FromType::AfterConfirmation:
			toValue = ToType::AfterConfirmation;
			return true;
		case FromType::Denied:
			toValue = ToType::Denied;
			return true;
	}

	return false;
}

} // namespace AccessControlService
} // namespace TVRemoteScreenSDKCommunication
