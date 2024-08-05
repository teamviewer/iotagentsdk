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
#pragma once

#include "IModule.h"
#include <TVAgentAPI/Callback.h>

#include <cstdint>

namespace tvagentapi
{

class IAugmentRCSessionModule : public IModule
{
public:
	/// Type value for convenience
	static constexpr Type TypeValue = Type::AugmentRCSession;

	using ReceivedInvitationCallback = Callback<void(const char* url, void* userdata) noexcept>;

	struct Callbacks
	{
		ReceivedInvitationCallback receivedInvitationCallback;
	};

	~IAugmentRCSessionModule() override = default;

	/**
	 * @brief setCallbacks sets callbacks to handle AugmentRCSession invitations.
	 * @param callbacks Might be partially or completely omitted, by providing default constructed Callback {}
	 */
	virtual void setCallbacks(const Callbacks& callbacks) = 0;

	/**
	 * @brief augmentRCSessionStartListening announces that module starts listening for AugmentRCSession invitations.
	 * @return false on internal error, true otherwise.
	 */
	virtual bool augmentRCSessionStartListening() = 0;

	/**
	 * @brief augmentRCSessionStopListening announces that module stops listening for AugmentRCSession invitations.
	 * @return false on internal error, true otherwise.
	 */
	virtual bool augmentRCSessionStopListening() = 0;
};

} // namespace tvagentapi
