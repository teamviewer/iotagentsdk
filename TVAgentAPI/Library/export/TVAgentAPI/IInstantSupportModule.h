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

class IInstantSupportModule : public IModule
{
public:
	/// Instant Support session state
	enum class SessionState : int32_t
	{
		/// There is no session
		Undefined = 0,
		/// Session is opened
		Open,
		/// Session is closed
		Closed,
	};

	/// Instant Support session data
	struct SessionData
	{
		/// Instant Support session code
		const char* sessionCode;
		/// Instant Support session name
		const char* name;
		/// Instant Support session description
		const char* description;
		/// Instant Support session state
		SessionState state;
	};

	/// Errors received from requestInstantSupport()
	enum class RequestErrorCode : int32_t
	{
		/// Generic error
		InternalError = 0,
		/// Access token is invalid
		InvalidToken,
		/// Group is invalid
		InvalidGroup,
		/// Session code is invalid
		InvalidSessionCode,
		/// Support was already requested
		Busy,
		/// Email is in wrong format
		InvalidEmail,
	};

	using SessionDataChangedCallback = Callback<void(SessionData newData, void* userdata) noexcept>;
	using RequestInstantSupportErrorCallback = Callback<void(RequestErrorCode errorCode, void* userdata) noexcept>;
	using InstantSupportConnectionRequestCallback = Callback<void(void* userdata) noexcept>;

	struct Callbacks
	{
		SessionDataChangedCallback sessionDataChangedCallback;
		RequestInstantSupportErrorCallback requestErrorCallback;
		InstantSupportConnectionRequestCallback connectionRequestCallback;
	};

	virtual ~IInstantSupportModule() = default;

	/**
	 * @brief setCallbacks sets callbacks to handle SessionData changes and requestInstantSupport() errors.
	 * @param callbacks Might be partially or completely omitted, by providing default constructed Callback {}
	 */
	virtual void setCallbacks(const Callbacks& callbacks) = 0;

	/**
	 * @brief requestInstantSupport requests an instant support service case.
	 * Set SessionDataChangedCallback and RequestInstantSupportErrorCallback to handle the result of this request.
	 * @param accessToken User or company access token for authentication.
	 * @param name Name of the end customer. Maximum length is 100 characters.
	 * @param group Name of the group the session code will be inserted into.
	 * @param description Description for the service case.
	 * @param email Customer email. Optional, might be omitted by providing nullptr.
	 * @param sessionCode Session code. Optional, might be omitted by providing nullptr.
	 * If it is not empty then it will be checked to be valid, otherwise a new session code will be created.
	 * @return false on request error, true otherwise
	 * all the response errors will be provided via RequestInstantSupportErrorCallback.
	 */
	virtual bool requestInstantSupport(
		const char* accessToken,
		const char* name,
		const char* group,
		const char* description,
		const char* email,
		const char* sessionCode) = 0;

	/**
	 * acceptConnectionRequest accepts the incoming instant support connection request.
	 * @return false on internal error, true otherwise
	 */
	virtual bool acceptConnectionRequest() = 0;

	/**
	 * rejectConnectionRequest rejects the incoming instant support connection request.
	 * @return false on internal error, true otherwise
	 */
	virtual bool rejectConnectionRequest() = 0;

	/**
	 * timeoutConnectionRequest marks the incoming instant support connection request as timed out.
	 * @return false on internal error, true otherwise
	 */
	virtual bool timeoutConnectionRequest() = 0;
};

} // namespace tvagentapi
