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
#include "PyTVAgentAPI.h"

#include <TVAgentAPI/tvagentapi.h>
#include <TVAgentAPI/ILogging.h>

#include "PyAccessControlModule.h"
#include "PyAgentConnection.h"
#include "PyInstantSupportModule.h"
#include "PyLogging.h"
#include "PythonHelpers.h"
#include "PyTVSessionManagementModule.h"
#include "Typesystem.h"

#include <new>

using namespace tvagentapipy;

namespace
{

int PyTVAgentAPI_init(PyTVAgentAPI* self, PyObject* args, PyObject* kwargs)
{
	(void)args;
	(void)kwargs;
	new(self) PyTVAgentAPI{};
	return self->IsReady() ? 0 : -1;
}

// Methods
PyObject* PyTVAgentAPI_createAgentConnectionLocal(PyTVAgentAPI* self, PyObject* args)
{
	PyLogging* pyLogging = nullptr;

	PyObject* arg = Py_None;
	// parse optional argument (if not present, 'arg' won't be initialized)
	if (!PyArg_ParseTuple(args, "|O", &arg))
	{
		return nullptr;
	}

	if (arg != Py_None)
	{
		PyTypeObject* pyTypeObject = GetPyTypeObject<PyLogging>();
		if (!PyObject_IsInstance(arg, reinterpret_cast<PyObject*>(pyTypeObject)))
		{
			PyErr_Format(
				PyExc_TypeError,
				"expected None or %s instance, got %R",
				pyTypeObject->tp_name,
				arg);
			return nullptr;
		}
		pyLogging = reinterpret_cast<PyLogging*>(arg);
	}

	auto pyAgentConnection = MakeWrapperObject<PyAgentConnection>(self, pyLogging);

	return reinterpret_cast<PyObject*>(pyAgentConnection);
}

PyObject* PyTVAgentAPI_createFileLogging(PyTVAgentAPI* self, PyObject* arg)
{
	if (!PyObject_IsInstance(arg, reinterpret_cast<PyObject*>(&PyUnicode_Type)))
	{
		PyErr_Format(PyExc_TypeError, "expected string, got %R", arg);
		return nullptr;
	}

	// this char buffer is being managed by python, no need to free it.
	const char* logFilePath = PyUnicode_AsUTF8(arg);

	auto pyLogging = MakeWrapperObject<PyLogging>(self, logFilePath);

	return reinterpret_cast<PyObject*>(pyLogging);
}

PyMethodDef PyTVAgentAPI_methods[] =
{
	{
		"createFileLogging",
		reinterpret_cast<PyCFunction>(PyTVAgentAPI_createFileLogging),
		METH_O,
		"create logging into a file"
	},

	{
		"createAgentConnectionLocal",
		reinterpret_cast<PyCFunction>(PyTVAgentAPI_createAgentConnectionLocal),
		METH_VARARGS,
		"create local connection"
	},

	{} // Sentinel
};

} // namespace

namespace tvagentapipy
{

PyTVAgentAPI::PyTVAgentAPI()
{
	m_agentAPI = TVGetAgentAPI();
}

PyTVAgentAPI::~PyTVAgentAPI()
{
	if (m_agentAPI)
	{
		TVDestroyAgentAPI();
	}
}

bool PyTVAgentAPI::IsReady() const
{
	return !!m_agentAPI;
}

PyTypeObject* GetPyTypeTVAgentAPI()
{
	static PyTypeObject* pyTypeTVAgentAPI = []() -> PyTypeObject*
	{
		static PyTypeObject result = PyTypeObjectInitialized();
		result.tp_name = "tvagentapi.TVAgentAPI";

		result.tp_new = PyType_GenericNew;
		result.tp_init = reinterpret_cast<initproc>(PyTVAgentAPI_init);
		result.tp_basicsize = sizeof(PyTVAgentAPI);
		result.tp_dealloc =
			reinterpret_cast<destructor>(DeallocWrapperObject<PyTVAgentAPI>);
		result.tp_flags = Py_TPFLAGS_DEFAULT;
		result.tp_doc = "TV Agent API class";
		result.tp_methods = PyTVAgentAPI_methods;

		if (PyType_Ready(&result) < 0)
		{
			return nullptr;
		}

		return &result;
	}();
	return pyTypeTVAgentAPI;
}

} // namespace tvagentapipy
