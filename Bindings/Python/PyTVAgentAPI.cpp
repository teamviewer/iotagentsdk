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
#include "PyAugmentRCSessionModule.h"
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
PyObject* PyTVAgentAPI_createAgentConnection(PyTVAgentAPI* self, PyObject* args)
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

PyObject* PyTVAgentAPI_createAgentConnectionLocal(PyTVAgentAPI* self, PyObject* args)
{
	PyErr_Warn(PyExc_DeprecationWarning, "use createAgentConnection");
	return PyTVAgentAPI_createAgentConnection(self, args);
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

namespace DocStrings
{

PyDoc_STRVAR(createFileLogging,
R"__(createFileLogging($self, logFilePath)
--

Creates a simple file logging object that creates a file and appends logs to it.
FileLogging is thread safe and its methods can be safely called from user code.

:param str logFilePath: path to log file location.
:return file logging object.
)__");

PyDoc_STRVAR(createAgentConnection,
R"__(createAgentConnection($self, logger)
--

Creates an instance of a connection to the running TeamViewer IoT Agent instance.
By default the connection already has the parameters for connecting to the agent.
You can change them via :p setConnectionURLs method before starting the connection.
The returned connection is to be used in subsequent calls to create functionality modules
(e.g. Instant Support, TV Session Management, Access Control, etc.).

:param logger: used internally for logging; logger ownership remains on caller's side and is not transferred to the connection.
If no logger is passed no logging happens.
:return agent connection object.
)__");

PyDoc_STRVAR(createAgentConnectionLocal,
R"__(createAgentConnectionLocal($self, logger)
--

Creates an instance of a connection to the running TeamViewer IoT Agent instance.
The returned connection is to be used in subsequent calls to create functionality modules
(e.g. Instant Support, TV Session Management, Access Control, etc.).

:param logger: would be used internally for logging; logging ownership remains on caller side and is not transferred to the connection.
If no logger is passed no logging happens.
:return agent connection object.
:deprecated use createAgentConnection.
)__");

} // namespace DocStrings

PyMethodDef PyTVAgentAPI_methods[] =
{
	{
		"createFileLogging",
		PyCFunctionCast(PyTVAgentAPI_createFileLogging),
		METH_O,
		DocStrings::createFileLogging
	},

	{
		"createAgentConnection",
		PyCFunctionCast(PyTVAgentAPI_createAgentConnection),
		METH_VARARGS,
		DocStrings::createAgentConnection
	},

	{
		"createAgentConnectionLocal",
		PyCFunctionCast(PyTVAgentAPI_createAgentConnectionLocal),
		METH_VARARGS,
		DocStrings::createAgentConnectionLocal
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
