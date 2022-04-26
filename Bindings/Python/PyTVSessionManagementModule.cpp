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
#include "PyTVSessionManagementModule.h"

#include <TVAgentAPI/IAgentAPI.h>
#include <TVAgentAPI/IAgentConnection.h>
#include <TVAgentAPI/ITVSessionManagementModule.h>

#include "PyAgentConnection.h"
#include "PyTVAgentAPI.h"
#include "PythonHelpers.h"
#include "Typesystem.h"

using namespace tvagentapipy;

namespace
{

// Methods

PyObject* PyTVSessionManagementModule_setCallbacks(
	PyTVSessionManagementModule* self,
	PyObject* arg)
{
	if (!PyDict_Check(arg))
	{
		PyErr_Format(PyExc_TypeError, "expected dict instance, got %R", arg);
		return nullptr;
	}

	PyObject* pySessionStartedCallback = PyDict_GetItemString(arg, "sessionStartedCallback");
	PyObject* pySessionStoppedCallback = PyDict_GetItemString(arg, "sessionStoppedCallback");

	if (pySessionStartedCallback && !PyCallable_Check(pySessionStartedCallback))
	{
		PyErr_Format(PyExc_TypeError, "%R is not callable", pySessionStartedCallback);
		return nullptr;
	}

	if (pySessionStoppedCallback && !PyCallable_Check(pySessionStoppedCallback))
	{
		PyErr_Format(PyExc_TypeError, "%R is not callable", pySessionStoppedCallback);
		return nullptr;
	}

	Py_XDECREF(self->m_pySessionStartedCallback);
	Py_XDECREF(self->m_pySessionStoppedCallback);
	self->m_pySessionStartedCallback = pySessionStartedCallback;
	self->m_pySessionStoppedCallback = pySessionStoppedCallback;
	Py_XINCREF(self->m_pySessionStartedCallback);
	Py_XINCREF(self->m_pySessionStoppedCallback);

	auto sessionCallback = [](
		tvagentapi::ITVSessionManagementModule::TVSessionID tvSessionID,
		int32_t tvSessionCount,
		void* userdata)
	{
		auto pySessionCallback = static_cast<PyObject*>(userdata);

		// TODO When Python >= 3.9 use PyObject_CallOneArg
		PyObject* args = Py_BuildValue(
			"(ii)",
			static_cast<int>(tvSessionID),
			tvSessionCount);
		PyObject* result = PyObject_CallObject(pySessionCallback, args);
		Py_XDECREF(args);
		Py_XDECREF(result);
	};

	tvagentapi::ITVSessionManagementModule::SessionCallback sessionStarted{};
	if (self->m_pySessionStartedCallback)
	{
		sessionStarted = {sessionCallback, self->m_pySessionStartedCallback};
	}

	tvagentapi::ITVSessionManagementModule::SessionCallback sessionStopped{};
	if (self->m_pySessionStoppedCallback)
	{
		sessionStopped = {sessionCallback, self->m_pySessionStoppedCallback};
	}

	self->m_module->setCallbacks({sessionStarted, sessionStopped});

	Py_RETURN_NONE;
}

PyObject* PyTVSessionManagementModule_getRunningSessions(
	PyTVSessionManagementModule* self,
	PyObject* args)
{
	(void)args;

	auto enumerator = [](
		tvagentapi::ITVSessionManagementModule::TVSessionID tvSessionID,
		void* userdata) -> bool
	{
		auto pyList = static_cast<PyObject*>(userdata);

		PyObject* pyTVSessionID = PyLong_FromLong(static_cast<long>(tvSessionID));
		PyList_Append(pyList, pyTVSessionID);
		Py_DECREF(pyTVSessionID);

		return true;
	};

	PyObject* pyList = PyList_New(/*size=*/0);

	if (!self->m_module->enumerateRunningSessions(
		tvagentapi::ITVSessionManagementModule::SessionEnumerator{enumerator, pyList}))
	{
		Py_DECREF(pyList);
		PyErr_SetString(PyExc_RuntimeError, "Failed to fetch the running sessions");
		return nullptr;
	}

	return pyList;
}

PyObject* PyTVSessionManagementModule_terminateTeamViewerSessions(
	PyTVSessionManagementModule* self,
	PyObject* args)
{
	(void)args;
	return NoneOrInternalError(self->m_module->terminateTeamViewerSessions());
}

PyObject* PyTVSessionManagementModule_isSupported(
	PyTVSessionManagementModule* self,
	PyObject* args)
{
	(void)args;
	if (self->m_module->isSupported())
	{
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

PyMethodDef PyTVSessionManagementModule_methods[] =
{
	{
		"setCallbacks",
		WeakConnectionCall<PyTVSessionManagementModule, PyTVSessionManagementModule_setCallbacks>,
		METH_O,
		"set TV session management callbacks"
	},

	{
		"getRunningSessions",
		WeakConnectionCall<PyTVSessionManagementModule, PyTVSessionManagementModule_getRunningSessions>,
		METH_NOARGS,
		"get running TV session IDs"
	},

	{
		"terminateTeamViewerSessions",
		WeakConnectionCall<PyTVSessionManagementModule, PyTVSessionManagementModule_terminateTeamViewerSessions>,
		METH_NOARGS,
		"terminate all TV sessions"
	},

	{
		"isSupported",
		WeakConnectionCall<PyTVSessionManagementModule, PyTVSessionManagementModule_isSupported>,
		METH_NOARGS,
		"check if the module is supported"
	},

	{} // Sentinel
};

} // namespace

namespace tvagentapipy
{

PyTVSessionManagementModule::PyTVSessionManagementModule(PyAgentConnection* pyAgentConnection)
	: m_pyWeakAgentConnection{reinterpret_cast<PyObject*>(pyAgentConnection)}
{
	m_module = static_cast<tvagentapi::ITVSessionManagementModule*>(
		pyAgentConnection->m_connection->getModule(
			tvagentapi::IModule::Type::TVSessionManagement
		));
}

PyTVSessionManagementModule::~PyTVSessionManagementModule()
{
	Py_XDECREF(m_pySessionStartedCallback);
	Py_XDECREF(m_pySessionStoppedCallback);
}

bool PyTVSessionManagementModule::IsReady() const
{
	return !!m_module;
}

PyTypeObject* GetPyTypeTVSessionManagementModule()
{
	static PyTypeObject* pyTVSessionManagementType = []() -> PyTypeObject*
	{
		static PyTypeObject result = PyTypeObjectInitialized();
		result.tp_name = "tvagentapi.TVSessionManagementModule";
		result.tp_basicsize = sizeof(PyTVSessionManagementModule);
		result.tp_dealloc =
			reinterpret_cast<destructor>(DeallocWrapperObject<PyTVSessionManagementModule>);
		result.tp_flags = Py_TPFLAGS_DEFAULT;
		result.tp_doc = "TVSessionManagement module class";
		result.tp_methods = PyTVSessionManagementModule_methods;

		if (PyType_Ready(&result) < 0)
		{
			return nullptr;
		}

		return &result;
	}();
	return pyTVSessionManagementType;
}

} // namespace tvagentapipy
