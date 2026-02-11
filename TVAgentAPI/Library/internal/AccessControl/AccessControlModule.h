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

#include <TVAgentAPI/IAccessControlModule.h>
#include <TVAgentAPIPrivate/Observer.h>

#include <memory>

namespace tvagentapi
{
class AgentConnection;

class AccessControlModule final : public IAccessControlModule
{
public:
	static std::shared_ptr<AccessControlModule> Create(std::weak_ptr<AgentConnection> connection);
	~AccessControlModule() override = default;

	void setCallbacks(const Callbacks& callbacks) override;

	bool getAccess(Feature feature, Access& outAccess) override;
	bool setAccess(Feature feature, Access access) override;

	bool acceptAccessRequest(Feature feature) override;
	bool rejectAccessRequest(Feature feature) override;

	bool setControlMode(ControlMode controlMode) override;

	// IModule
	bool isSupported() const override;

private:
	AccessControlModule(std::weak_ptr<AgentConnection> connection);
	bool registerCallbacks();

	std::weak_ptr<AccessControlModule> m_weakThis;
	std::weak_ptr<AgentConnection> m_connection;

	ObserverConnection m_accessChangeNotificationConnection;
	ObserverConnection m_accessConfirmationRequestedConnection;

	Callbacks m_callbacks;
};

} // namespace tvagentapi
