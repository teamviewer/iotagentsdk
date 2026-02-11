//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2022 TeamViewer Germany GmbH                                     //
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

#include "IModule.h"
#include <TVAgentAPI/Callback.h>

#include <cstdint>

namespace tvagentapi
{

class IAccessControlModule : public IModule
{
public:
	/// Type value for convenience
	static constexpr Type TypeValue = Type::AccessControl;

	enum class Feature : int32_t
	{
		/// File Transfer access feature, partner can transfer files to the machine.
		FileTransfer = 0,
		/// Screen sharing, partner can view the screen of the machine.
		/// If AllowRemoteView/RemoteView feature is Denied, remote controlling the machine is not possible.
		RemoteView = 1,
		/// Partner can control the machine.
		RemoteControl = 2,
		/// Partner can record the remote screen session
		ScreenRecording = 3,
	};

	enum class Access : int32_t
	{
		/// Access allowed.
		Allowed = 0,
		/// Access mode determined by reply to the confirmation request.
		AfterConfirmation = 1,
		/// Access denied.
		Denied = 2,
	};

	enum class ControlMode : int32_t
	{
		Disabled = 0,
		ViewOnly = 1,
		FullControl = 2,
	};

	using AccessChangedCallback = Callback<void(Feature feature, Access access, void* userdata) noexcept>;
	using AccessConfirmationRequestCallback = Callback<void(Feature feature, void* userdata) noexcept>;

	struct Callbacks
	{
		AccessChangedCallback accessChangedCallback;
		AccessConfirmationRequestCallback accessRequestCallback;
	};

	~IAccessControlModule() override = default;

	/**
	 * @brief setCallbacks sets callbacks to handle Access changes and incoming access confirmation requests.
	 * @param callbacks Might be partially or completely omitted, by providing default constructed Callback {}
	 */
	virtual void setCallbacks(const Callbacks& callbacks) = 0;

	/**
	 * @brief getAccess requests the current access mode for the given feature.
	 * @param feature is the Feature value for which access mode is requested.
	 * @param outAccess contains the requested access mode, if the return value of this function is false then requestedAccess will stay unchanged.
	 * @return false on internal error, true otherwise.
	 */
	virtual bool getAccess(Feature feature, Access& outAccess) = 0;

	/**
	 * @brief setAccess requests to set the given access value for the given feature.
	 * @param feature is the Feature value to set the given access permission.
	 * @param access is the access mode value to set.
	 * @return false on internal error, true otherwise.
	 */
	virtual bool setAccess(Feature feature, Access access) = 0;

	/**
	 * @brief acceptAccessRequest accepts an incoming access request for the given feature.
	 * @param feature for which request is accepted.
	 * @return false on internal error, true otherwise.
	 */
	virtual bool acceptAccessRequest(Feature feature) = 0;

	/**
	 * @brief rejectAccessRequest rejects an incoming access request for the given feature.
	 * @param feature for which request is rejected.
	 * @return false on internal error, true otherwise.
	 */
	virtual bool rejectAccessRequest(Feature feature) = 0;

	virtual bool setControlMode(ControlMode controlMode) = 0;
};

} // namespace tvagentapi
