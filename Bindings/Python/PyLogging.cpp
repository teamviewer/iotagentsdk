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
#include "PyLogging.h"

#include <TVAgentAPI/IAgentAPI.h>
#include <TVAgentAPI/ILogging.h>

#include "PythonHelpers.h"
#include "PyTVAgentAPI.h"
#include "Typesystem.h"

using namespace tvagentapipy;

namespace
{

// Methods
PyObject* PyLogging_logInfo(PyLogging* self, PyObject* arg)
{
	if (!PyUnicode_Check(arg))
	{
		PyErr_Format(PyExc_TypeError, "expected string, got %R", arg);
		return nullptr;
	}
	self->m_logging->logInfo(PyUnicode_AsUTF8(arg));
	Py_RETURN_NONE;
}

PyObject* PyLogging_logError(PyLogging* self, PyObject* arg)
{
	if (!PyUnicode_Check(arg))
	{
		PyErr_Format(PyExc_TypeError, "expected string, got %R", arg);
		return nullptr;
	}
	self->m_logging->logError(PyUnicode_AsUTF8(arg));
	Py_RETURN_NONE;
}

namespace DocStrings
{

PyDoc_STRVAR(logInfo,
R"__(logInfo($self, info)
--

Log an information message.

:param str info: information message
)__");

PyDoc_STRVAR(logError,
R"__(logError($self, error)
--

Log an error message.

:param str info: error message
)__");

} // namespace DocStrings

PyMethodDef PyLogging_methods[] =
{
	{
		"logInfo",
		PyCFunctionCast(PyLogging_logInfo),
		METH_O,
		DocStrings::logInfo
	},

	{
		"logError",
		PyCFunctionCast(PyLogging_logError),
		METH_O,
		DocStrings::logError
	},

	{} // Sentinel
};

} // namespace

namespace tvagentapipy
{

PyLogging::PyLogging(PyTVAgentAPI* pyAgentAPI, const char* logFilePath)
	: m_pyAgentAPI{pyAgentAPI}
{
	m_logging = pyAgentAPI->m_agentAPI->createFileLogging(logFilePath);
	Py_INCREF(m_pyAgentAPI);
}

PyLogging::~PyLogging()
{
	m_pyAgentAPI->m_agentAPI->destroyLogging(m_logging);
	Py_DECREF(m_pyAgentAPI);
}

bool PyLogging::IsReady() const
{
	return !!m_logging;
}

PyTypeObject* GetPyTypeLogging()
{
	static PyTypeObject* pyTypeTVAgentAPI = []() -> PyTypeObject*
	{
		static PyTypeObject result = PyTypeObjectInitialized();
		result.tp_name = "tvagentapi.Logging";
		result.tp_basicsize = sizeof(PyLogging);
		result.tp_dealloc =
			reinterpret_cast<destructor>(DeallocWrapperObject<PyLogging>);
		result.tp_flags = Py_TPFLAGS_DEFAULT;
		result.tp_doc = "Logging class";
		result.tp_methods = PyLogging_methods;

		if (PyType_Ready(&result) < 0)
		{
			return nullptr;
		}

		return &result;
	}();
	return pyTypeTVAgentAPI;
}

} // namespace tvagentapipy
