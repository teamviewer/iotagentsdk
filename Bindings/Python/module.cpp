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
#include "PyAccessControlModule.h"
#include "PyTVAgentAPI.h"
#include "PyAgentConnection.h"
#include "PyInstantSupportModule.h"
#include "PyLogging.h"
#include "PyTVSessionManagementModule.h"
#include "Typesystem.h"

#include <TVAgentAPI/IModule.h>

using namespace tvagentapipy;

namespace
{

const char* docstring = "tvagentapi module doc";

PyModuleDef tvagentModule =
{
	PyModuleDef_HEAD_INIT,
	"tvagentapi",  // name of module
	docstring,  // module documentation
	-1,         // size of per-interpreter state of the module or -1 if the module keeps state in global variables.
	nullptr, nullptr, nullptr, nullptr, nullptr
};

} // namespace

PyMODINIT_FUNC PyInit_tvagentapi(void)
{
	const PyTypeMeta exposedTypeMetas[] =
	{
		GetPyTypeMeta<PyTVAgentAPI>(),
		GetPyTypeMeta<PyLogging>(),
		GetPyTypeMeta<PyAgentConnection>(),
		GetPyTypeMeta<PyAccessControlModule>(),
		GetPyTypeMeta<PyInstantSupportModule>(),
		GetPyTypeMeta<PyTVSessionManagementModule>(),
		GetPyTypeMeta<tvagentapi::IModule::Type>(),
	};

	for (const auto& exposedMeta : exposedTypeMetas)
	{
		if (exposedMeta.pyTypeObject == nullptr)
		{
			PyErr_Format(
				PyExc_SystemError,
				"Failed to initialize tvagentapi.%s",
				exposedMeta.publicName);
			return nullptr;
		}
	}

	PyObject* module = PyModule_Create(&tvagentModule);
	if (module == nullptr)
	{
		return nullptr;
	}

	for (const auto& exposedMeta : exposedTypeMetas)
	{
		Py_INCREF(exposedMeta.pyTypeObject);
		// See https://docs.python.org/3.8/c-api/module.html#c.PyModule_AddObject
		const int result = PyModule_AddObject(
			module,
			exposedMeta.publicName,
			reinterpret_cast<PyObject*>(exposedMeta.pyTypeObject));
		if (result < 0)
		{
			Py_DECREF(exposedMeta.pyTypeObject);
			Py_DECREF(module);
			PyErr_Format(
				PyExc_SystemError,
				"Failed to add %s",
				exposedMeta.pyTypeObject->tp_name);
			return nullptr;
		}
	}

	return module;
}
