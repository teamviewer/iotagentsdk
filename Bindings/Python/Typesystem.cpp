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
#include "Typesystem.h"

#include <TVAgentAPI/IAccessControlModule.h>

#include "PyAccessControlModule.h"
#include "PyAgentConnection.h"
#include "PyInstantSupportModule.h"
#include "PyLogging.h"
#include "PyModuleType.h"
#include "PyTVAgentAPI.h"
#include "PyTVSessionManagementModule.h"

namespace tvagentapipy
{

template<>
PyTypeMeta GetPyTypeMeta<PyAgentConnection>()
{
	PyTypeMeta meta{GetPyTypeAgentConnection(), "AgentConnection"};
	return meta;
}

template<>
PyTypeMeta GetPyTypeMeta<PyAccessControlModule>()
{
	PyTypeMeta meta{GetPyTypeAccessControlModule(), "AccessControlModule"};
	return meta;
}

template<>
PyTypeMeta GetPyTypeMeta<PyInstantSupportModule>()
{
	PyTypeMeta meta{GetPyTypeInstantSupportModule(), "InstantSupportModule"};
	return meta;
}

template<>
PyTypeMeta GetPyTypeMeta<PyLogging>()
{
	PyTypeMeta meta{GetPyTypeLogging(), "Logging"};
	return meta;
}

template<>
PyTypeMeta GetPyTypeMeta<PyTVAgentAPI>()
{
	PyTypeMeta meta{GetPyTypeTVAgentAPI(), "TVAgentAPI"};
	return meta;
}

template<>
PyTypeMeta GetPyTypeMeta<PyTVSessionManagementModule>()
{
	PyTypeMeta meta{GetPyTypeTVSessionManagementModule(), "TVSessionManagementModule"};
	return meta;
}

template<>
PyTypeMeta GetPyTypeMeta<tvagentapi::IAccessControlModule::Feature>()
{
	PyTypeMeta meta{GetPyTypeAccessControlModule_Feature(), "Feature"};
	return meta;
}

template<>
PyTypeMeta GetPyTypeMeta<tvagentapi::IAccessControlModule::Access>()
{
	PyTypeMeta meta{GetPyTypeAccessControlModule_Access(), "Access"};
	return meta;
}

template<>
PyTypeMeta GetPyTypeMeta<tvagentapi::IModule::Type>()
{
	PyTypeMeta meta{GetPyTypeModule_Type(), "ModuleType"};
	return meta;
}

} // namespace tvagentapipy
