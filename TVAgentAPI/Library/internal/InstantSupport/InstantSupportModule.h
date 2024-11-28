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

#include "TVAgentAPI/IInstantSupportModule.h"
#include <TVAgentAPIPrivate/Observer.h>

#include <memory>

namespace tvagentapi
{
class AgentConnection;

class InstantSupportModule final : public IInstantSupportModule
{
public:
	static std::shared_ptr<InstantSupportModule> Create(std::weak_ptr<AgentConnection> connection);
	~InstantSupportModule() override = default;

	void setCallbacks(const Callbacks& callbacks) override;

	bool requestInstantSupport(
		const char* accessToken,
		const char* name,
		const char* group,
		const char* description,
		const char* email,
		const char* sessionCode) override;

	bool closeInstantSupportCase(
		const char* accessToken,
		const char* sessionCode) override;

	bool acceptConnectionRequest() override;
	bool rejectConnectionRequest() override;
	bool timeoutConnectionRequest() override;

	// IModule
	bool isSupported() const override;

private:
	InstantSupportModule(std::weak_ptr<AgentConnection> connection);
	bool registerCallbacks();

	std::weak_ptr<InstantSupportModule> m_weakThis;
	std::weak_ptr<AgentConnection> m_connection;

	ObserverConnection m_requestInstantSupportErrorNotificationConnection;
	ObserverConnection m_instantSupportModifiedNotificationConnection;
	ObserverConnection m_instantSupportRequestConnection;

	Callbacks m_callbacks;
};

} // namespace tvagentapi
