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
#include "ModuleFactory.h"

namespace tvagentapi
{

std::shared_ptr<IModule> CreateModule(IModule::Type moduleType, std::weak_ptr<AgentConnection> connection)
{
	switch (moduleType)
	{
		case IModule::Type::AccessControl: return CreateModule<IModule::Type::AccessControl>(std::move(connection));
		case IModule::Type::InstantSupport: return CreateModule<IModule::Type::InstantSupport>(std::move(connection));
		case IModule::Type::TVSessionManagement: return CreateModule<IModule::Type::TVSessionManagement>(std::move(connection));
		case IModule::Type::Chat: return CreateModule<IModule::Type::Chat>(std::move(connection));
		case IModule::Type::AugmentRCSession: return CreateModule<IModule::Type::AugmentRCSession>(std::move(connection));
	}
	return {};
}

} // namespace tvagentapi
