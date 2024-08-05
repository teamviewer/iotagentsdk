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
#include "PyAugmentRCSessionModule.h"
#include <TVAgentAPI/IAugmentRCSessionModule.h>
#include <TVAgentAPI/IAgentAPI.h>
#include <TVAgentAPI/IAgentConnection.h>

#include "PyAgentConnection.h"
#include "PyTVAgentAPI.h"
#include "PythonHelpers.h"
#include "Typesystem.h"

using namespace tvagentapipy;

namespace
{

// Methods

PyObject* PyAugmentRCSessionModule_setCallbacks(
	PyAugmentRCSessionModule* self,
	PyObject* args,
	PyObject* kwargs)
{
	char augmentRCSessionInvitationReceivedArgName[] = "augmentRCSessionInvitationReceivedCallback";

	PyObject* augmentRCSessionInvitationReceivedCallback = Py_None;

	char* kwargList[] = {augmentRCSessionInvitationReceivedArgName, {}};
	if (!PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"|O:setCallbacks",
		kwargList,
		&augmentRCSessionInvitationReceivedCallback))
	{
		return nullptr;
	}

	augmentRCSessionInvitationReceivedCallback = augmentRCSessionInvitationReceivedCallback == Py_None ? nullptr : augmentRCSessionInvitationReceivedCallback;

	if (augmentRCSessionInvitationReceivedCallback && !PyCallable_Check(augmentRCSessionInvitationReceivedCallback))
	{
		PyErr_Format(PyExc_TypeError, "%R is not callable", augmentRCSessionInvitationReceivedCallback);
		return nullptr;
	}

	Py_XINCREF(augmentRCSessionInvitationReceivedCallback);
	Py_XDECREF(self->m_pyAugmentRCSessionInvitationReceivedCallback);
	self->m_pyAugmentRCSessionInvitationReceivedCallback = augmentRCSessionInvitationReceivedCallback;

	auto augmentRCSessionInvitation = [](const char* url, void* userdata) noexcept
	{
		auto pyAugmentRCSessionInvitationReceivedCallback = static_cast<PyObject*>(userdata);

		PyObject* args = Py_BuildValue("(s)", url);

		PyObject* result = PyObject_CallObject(pyAugmentRCSessionInvitationReceivedCallback, args);
		Py_DECREF(args);
		Py_XDECREF(result);
	};

	tvagentapi::IAugmentRCSessionModule::ReceivedInvitationCallback invitationReceivedCb{};
	if (self->m_pyAugmentRCSessionInvitationReceivedCallback)
	{
		invitationReceivedCb = {augmentRCSessionInvitation, self->m_pyAugmentRCSessionInvitationReceivedCallback};
	}

	self->m_module->setCallbacks({invitationReceivedCb});

	Py_RETURN_NONE;
}

PyObject* PyAugmentRCSessionModule_startListening(PyAugmentRCSessionModule* self, PyObject* args)
{
	(void)args;
	return NoneOrInternalError(self->m_module->augmentRCSessionStartListening());
}

PyObject* PyAugmentRCSessionModule_stopListening(PyAugmentRCSessionModule* self, PyObject* args)
{
	(void)args;
	return NoneOrInternalError(self->m_module->augmentRCSessionStopListening());
}

PyObject* PyAugmentRCSessionModule_isSupported(PyAugmentRCSessionModule* self, PyObject* args)
{
	(void)args;
	if (self->m_module->isSupported())
	{
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

namespace DocStrings
{

PyDoc_STRVAR(isSupported,
R"__(isSupported($self)
--

Returns whether the current module is supported by the SDK and the IoT Agent counterpart.

:return True if the current module is supported, False otherwise.
)__");

PyDoc_STRVAR(setCallbacks,
R"__(setCallbacks($self, /,
	AugmentRCSessionInvitationReceived=None)
--

Sets callbacks to handle various events like chat room and message updates.

:param AugmentRCSessionInvitationReceivedCallback: called when an invitation to an Augment RC Session is received
:type AugmentRCSessionInvitationReceivedCallback: callback(str url)
)__");

PyDoc_STRVAR(startListening,
R"__(startListening($self)
--

Announces that the module starts listening for AugmentRCSession invitations.
)__");

PyDoc_STRVAR(stopListening,
R"__(stopListening($self)
--

Announces that the module stops listening for AugmentRCSession invitations.
)__");

} // namespace DocStrings

PyMethodDef PyAugmentRCSessionModule_methods[] =
{
	{
		"isSupported",
		WeakConnectionCall<PyAugmentRCSessionModule, PyAugmentRCSessionModule_isSupported>,
		METH_NOARGS,
		DocStrings::isSupported
	},
	{
		"setCallbacks",
		PyCFunctionCast(WeakConnectionCall<PyAugmentRCSessionModule, PyAugmentRCSessionModule_setCallbacks>),
		METH_VARARGS | METH_KEYWORDS,
		DocStrings::setCallbacks
	},
	{
		"startListening",
		WeakConnectionCall<PyAugmentRCSessionModule, PyAugmentRCSessionModule_startListening>,
		METH_NOARGS,
		DocStrings::startListening
	},

	{
		"stopListening",
		WeakConnectionCall<PyAugmentRCSessionModule, PyAugmentRCSessionModule_stopListening>,
		METH_NOARGS,
		DocStrings::stopListening
	},

	{} // Sentinel
};

} // namespace

namespace tvagentapipy
{

PyAugmentRCSessionModule::PyAugmentRCSessionModule(PyAgentConnection* pyAgentConnection)
	: m_pyWeakAgentConnection{reinterpret_cast<PyObject*>(pyAgentConnection)}
{
	m_module = static_cast<tvagentapi::IAugmentRCSessionModule*>(
		pyAgentConnection->m_connection->getModule(
			tvagentapi::IModule::Type::AugmentRCSession
		));
}

PyAugmentRCSessionModule::~PyAugmentRCSessionModule()
{
	Py_XDECREF(m_pyAugmentRCSessionInvitationReceivedCallback);
}

bool PyAugmentRCSessionModule::IsReady() const
{
	return !!m_module;
}

PyTypeObject* GetPyTypeAugmentRCSessionModule()
{
	static PyTypeObject* pyAugmentRCSessionModuleType = []() -> PyTypeObject*
	{
		static PyTypeObject result = PyTypeObjectInitialized();
		result.tp_name = "tvagentapi.AugmentRCSessionModule";
		result.tp_basicsize = sizeof(PyAugmentRCSessionModule);
		result.tp_dealloc = reinterpret_cast<destructor>(DeallocWrapperObject<PyAugmentRCSessionModule>);
		result.tp_flags = Py_TPFLAGS_DEFAULT;
		result.tp_doc = "AugmentRCSession module class";
		result.tp_methods = PyAugmentRCSessionModule_methods;

		if (PyType_Ready(&result) < 0)
		{
			return nullptr;
		}

		return &result;
	}();
	return pyAugmentRCSessionModuleType;
}

} // namespace tvagentapipy
