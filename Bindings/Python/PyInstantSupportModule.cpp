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
#include "PyInstantSupportModule.h"

#include <TVAgentAPI/IAgentAPI.h>
#include <TVAgentAPI/IAgentConnection.h>
#include <TVAgentAPI/IInstantSupportModule.h>
#include <TVAgentAPI/InstantSupportModuleStringify.h>

#include "PyAgentConnection.h"
#include "PyTVAgentAPI.h"
#include "PythonHelpers.h"
#include "Typesystem.h"

using namespace tvagentapipy;

namespace
{

// Methods

PyObject* PyInstantSupportModule_setCallbacks(
	PyInstantSupportModule* self,
	PyObject* args,
	PyObject* kwargs)
{
	char dataChangedCbName[] = "sessionDataChangedCallback";
	char requestErrorCbName[] = "requestErrorCallback";
	char connectionRequestCbName[] = "connectionRequestCallback";

	PyObject* sessionDataChangedCallback = Py_None;
	PyObject* requestErrorCallback = Py_None;
	PyObject* connectionRequestCallback = Py_None;

	char* kwargList[] = {dataChangedCbName, requestErrorCbName, connectionRequestCbName, {}};
	if (!PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"|OOO:setCallbacks",
		kwargList,
		&sessionDataChangedCallback,
		&requestErrorCallback,
		&connectionRequestCallback))
	{
		return nullptr;
	}

	sessionDataChangedCallback = sessionDataChangedCallback == Py_None ? nullptr : sessionDataChangedCallback;
	requestErrorCallback = requestErrorCallback == Py_None ? nullptr : requestErrorCallback;
	connectionRequestCallback = connectionRequestCallback == Py_None ? nullptr : connectionRequestCallback;

	if (sessionDataChangedCallback && !PyCallable_Check(sessionDataChangedCallback))
	{
		PyErr_Format(PyExc_TypeError, "%R is not callable", sessionDataChangedCallback);
		return nullptr;
	}

	if (requestErrorCallback && !PyCallable_Check(requestErrorCallback))
	{
		PyErr_Format(PyExc_TypeError, "%R is not callable", requestErrorCallback);
		return nullptr;
	}

	if (connectionRequestCallback && !PyCallable_Check(connectionRequestCallback))
	{
		PyErr_Format(PyExc_TypeError, "%R is not callable", connectionRequestCallback);
		return nullptr;
	}

	Py_XINCREF(sessionDataChangedCallback);
	Py_XINCREF(requestErrorCallback);
	Py_XINCREF(connectionRequestCallback);
	Py_XDECREF(self->m_pySessionDataChangedCallback);
	Py_XDECREF(self->m_pyRequestErrorCallback);
	Py_XDECREF(self->m_pyConnectionRequestCallback);
	self->m_pySessionDataChangedCallback = sessionDataChangedCallback;
	self->m_pyRequestErrorCallback = requestErrorCallback;
	self->m_pyConnectionRequestCallback = connectionRequestCallback;

	auto sessionDataChanged = [](
		tvagentapi::IInstantSupportModule::SessionData newData,
		void* userdata) noexcept
	{
		auto pySessionDataChangedCallback = static_cast<PyObject*>(userdata);
		PyObject* pyNewData = PyDict_New();

		{
			PyObject* pySessionCode = PyUnicode_FromString(newData.sessionCode);
			PyDict_SetItemString(pyNewData, "sessionCode", pySessionCode);
			Py_DECREF(pySessionCode);
		}

		{
			PyObject* pyName = PyUnicode_FromString(newData.name);
			PyDict_SetItemString(pyNewData, "name", pyName);
			Py_DECREF(pyName);
		}

		{
			PyObject* pyDescription = PyUnicode_FromString(newData.description);
			PyDict_SetItemString(pyNewData, "description", pyDescription);
			Py_DECREF(pyDescription);
		}

		PyObject* enumVal = PyEnumValue(
			GetPyTypeInstantSupportModule_SessionState(),
			tvagentapi::toCString(newData.state));

		PyDict_SetItemString(pyNewData, "state", enumVal);

		// TODO When Python >= 3.9 use PyObject_CallOneArg
		PyObject* args = Py_BuildValue("(O)", pyNewData);

		PyObject* result = PyObject_CallObject(pySessionDataChangedCallback, args);
		Py_XDECREF(result);
		Py_DECREF(args);
		Py_DECREF(enumVal);
		Py_DECREF(pyNewData);
	};

	auto requestError = [](
		tvagentapi::IInstantSupportModule::RequestErrorCode errorCode,
		void* userdata)
	{
		auto pyRequestErrorCallback = static_cast<PyObject*>(userdata);

		PyObject* enumVal = PyEnumValue(
			GetPyTypeInstantSupportModule_RequestErrorCode(),
			tvagentapi::toCString(errorCode));

		// TODO When Python >= 3.9 use PyObject_CallOneArg
		PyObject* args = Py_BuildValue("(O)", enumVal);

		PyObject* result = PyObject_CallObject(pyRequestErrorCallback, args);
		Py_XDECREF(result);
		Py_DECREF(args);
		Py_DECREF(enumVal);
	};

	auto connectionRequest = [](void* userdata)
	{
		auto pyConnectionRequestCallback = static_cast<PyObject*>(userdata);
		PyObject* result = PyObject_CallObject(pyConnectionRequestCallback, nullptr);
		Py_XDECREF(result);
	};

	tvagentapi::IInstantSupportModule::SessionDataChangedCallback sessionDataChangedCb{};
	if (self->m_pySessionDataChangedCallback)
	{
		sessionDataChangedCb = {sessionDataChanged, self->m_pySessionDataChangedCallback};
	}

	tvagentapi::IInstantSupportModule::RequestInstantSupportErrorCallback requestErrorCb{};
	if(self->m_pyRequestErrorCallback)
	{
		requestErrorCb = {requestError, self->m_pyRequestErrorCallback};
	}

	tvagentapi::IInstantSupportModule::InstantSupportConnectionRequestCallback connectionRequestCb{};
	if(self->m_pyConnectionRequestCallback)
	{
		connectionRequestCb = {connectionRequest, self->m_pyConnectionRequestCallback};
	}

	self->m_module->setCallbacks({sessionDataChangedCb, requestErrorCb, connectionRequestCb});

	Py_RETURN_NONE;
}

PyObject* PyInstantSupportModule_requestInstantSupport(PyInstantSupportModule* self, PyObject* arg)
{
	const char* accessTokenName = "accessToken";
	const char* nameName = "name";
	const char* groupName = "group";
	const char* descriptionName = "description";
	const char* emailName = "email";
	const char* sessionCodeName = "sessionCode";

	PyObject* pyAccessToken = PyDict_GetItemString(arg, accessTokenName);
	if (pyAccessToken == nullptr)
	{
		PyErr_SetString(PyExc_KeyError, accessTokenName);
		return nullptr;
	}

	PyObject* pyName = PyDict_GetItemString(arg, nameName);
	if (pyName == nullptr)
	{
		PyErr_SetString(PyExc_KeyError, nameName);
		return nullptr;
	}

	PyObject* pyGroup = PyDict_GetItemString(arg, groupName);
	if (pyGroup == nullptr)
	{
		PyErr_SetString(PyExc_KeyError, groupName);
		return nullptr;
	}

	PyObject* pyDescription = PyDict_GetItemString(arg, descriptionName);
	if (pyDescription == nullptr)
	{
		PyErr_SetString(PyExc_KeyError, descriptionName);
		return nullptr;
	}

	PyObject* pyEmail = PyDict_GetItemString(arg, emailName);
	PyObject* pySessionCode = PyDict_GetItemString(arg, sessionCodeName);

	return NoneOrInternalError(
		self->m_module->requestInstantSupport(
			PyUnicode_AsUTF8(pyAccessToken),
			PyUnicode_AsUTF8(pyName),
			PyUnicode_AsUTF8(pyGroup),
			PyUnicode_AsUTF8(pyDescription),
			pyEmail ? PyUnicode_AsUTF8(pyEmail) : nullptr,
			pySessionCode ? PyUnicode_AsUTF8(pySessionCode) : nullptr));
}

PyObject* PyInstantSupportModule_closeInstantSupportCase(PyInstantSupportModule* self, PyObject* arg)
{
	const char* accessTokenName = "accessToken";
	const char* sessionCodeName = "sessionCode";

	PyObject* pyAccessToken = PyDict_GetItemString(arg, accessTokenName);
	if (pyAccessToken == nullptr)
	{
		PyErr_SetString(PyExc_KeyError, accessTokenName);
		return nullptr;
	}

	PyObject* pySessionCode = PyDict_GetItemString(arg, sessionCodeName);

	return NoneOrInternalError(
		self->m_module->closeInstantSupportCase(
			PyUnicode_AsUTF8(pyAccessToken),
			pySessionCode ? PyUnicode_AsUTF8(pySessionCode) : nullptr));
}

PyObject* PyInstantSupportModule_isSupported(PyInstantSupportModule* self, PyObject* args)
{
	(void)args;
	if (self->m_module->isSupported())
	{
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

PyObject* PyInstantSupportModule_acceptConnectionRequest(PyInstantSupportModule* self, PyObject* args)
{
	(void)args;
	return NoneOrInternalError(self->m_module->acceptConnectionRequest());
}

PyObject* PyInstantSupportModule_rejectConnectionRequest(PyInstantSupportModule* self, PyObject* args)
{
	(void)args;
	return NoneOrInternalError(self->m_module->rejectConnectionRequest());
}

PyObject* PyInstantSupportModule_timeoutConnectionRequest(PyInstantSupportModule* self, PyObject* args)
{
	(void)args;
	return NoneOrInternalError(self->m_module->timeoutConnectionRequest());
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
R"__(setCallbacks($self, sessionDataChangedCallback=None, requestErrorCallback=None,connectionRequestCallback=None)
--

Set instant support callbacks.

:param sessionDataChangedCallback: called when there is a change in the instant support session data
:type sessionDataChangedCallback: callback(dict new_data {'sessionCode' => str, 'name' => str, 'description' => str, 'state' => tvagentapi.InstantSupportModule.SessionState})
:param requestErrorCallback: called when the request for instant support has resulted in an error
:type requestErrorCallback: callback(tvagentapi.InstantSupportModule.RequestErrorCode error_code)
:param connectionRequestCallback: called when the remote party instant support was requested from attempts to make an inbound connection
:type connectionRequestCallback: callback()
)__");

PyDoc_STRVAR(requestInstantSupport,
R"__(requestInstantSupport($self, accessToken, name, group, description, email, sessionCode)
--

Requests an instant support service case. Parameters are passed as keyword arguments.
Set SessionDataChangedCallback and RequestInstantSupportErrorCallback to handle the result of this request.

:param str accessToken: User or company access token for authentication.
:param str name: Name of the end customer. Maximum length is 100 characters.
:param str group: Name of the group the session code will be inserted into.
:param str description: Description for the service case.
:param str email: Customer email (optional).
:param str sessionCode: Session code (optional). Will be checked for validity; leave empty to create new session code.
)__");

PyDoc_STRVAR(closeInstantSupportCase,
R"__(closeInstantSupportCase($self, accessToken, sessionCode)
--

Requests an instant support service case close. Parameters are passed as keyword arguments.
Set SessionDataChangedCallback and RequestInstantSupportErrorCallback to handle the result of this request.

:param str accessToken: User or company access token for authentication.
:param str sessionCode: Session code.
)__");

PyDoc_STRVAR(acceptConnectionRequest,
R"__(acceptConnectionRequest($self)
--

Accept the incoming instant support connection request.
)__");

PyDoc_STRVAR(rejectConnectionRequest,
R"__(rejectConnectionRequest($self)
--

Reject the incoming instant support connection request.
)__");

PyDoc_STRVAR(timeoutConnectionRequest,
R"__(timeoutConnectionRequest($self)
--

Timeout the incoming instant support connection request.
)__");


} // namespace DocStrings

PyMethodDef PyInstantSupportModule_methods[] =
{
	{
		"isSupported",
		WeakConnectionCall<PyInstantSupportModule, PyInstantSupportModule_isSupported>,
		METH_NOARGS,
		DocStrings::isSupported
	},

	{
		"setCallbacks",
		PyCFunctionCast(WeakConnectionCall<PyInstantSupportModule, PyInstantSupportModule_setCallbacks>),
		METH_VARARGS | METH_KEYWORDS,
		DocStrings::setCallbacks
	},

	{
		"requestInstantSupport",
		WeakConnectionCall<PyInstantSupportModule, PyInstantSupportModule_requestInstantSupport>,
		METH_O,
		DocStrings::requestInstantSupport
	},

	{
		"closeInstantSupportCase",
		WeakConnectionCall<PyInstantSupportModule, PyInstantSupportModule_closeInstantSupportCase>,
		METH_O,
		DocStrings::closeInstantSupportCase
	},

	{
		"acceptConnectionRequest",
		WeakConnectionCall<PyInstantSupportModule, PyInstantSupportModule_acceptConnectionRequest>,
		METH_NOARGS,
		DocStrings::acceptConnectionRequest
	},

	{
		"rejectConnectionRequest",
		WeakConnectionCall<PyInstantSupportModule, PyInstantSupportModule_rejectConnectionRequest>,
		METH_NOARGS,
		DocStrings::rejectConnectionRequest
	},

	{
		"timeoutConnectionRequest",
		WeakConnectionCall<PyInstantSupportModule, PyInstantSupportModule_timeoutConnectionRequest>,
		METH_NOARGS,
		DocStrings::timeoutConnectionRequest
	},

	{} // Sentinel
};

} // namespace

namespace tvagentapipy
{

PyInstantSupportModule::PyInstantSupportModule(PyAgentConnection* pyAgentConnection)
	: m_pyWeakAgentConnection{reinterpret_cast<PyObject*>(pyAgentConnection)}
{
	m_module = static_cast<tvagentapi::IInstantSupportModule*>(
		pyAgentConnection->m_connection->getModule(
			tvagentapi::IModule::Type::InstantSupport
		));
}

PyInstantSupportModule::~PyInstantSupportModule()
{
	Py_XDECREF(m_pySessionDataChangedCallback);
	Py_XDECREF(m_pyRequestErrorCallback);
	Py_XDECREF(m_pyConnectionRequestCallback);
}

bool PyInstantSupportModule::IsReady() const
{
	return !!m_module;
}

PyTypeObject* GetPyTypeInstantSupportModule()
{
	static PyTypeObject* pyInstantSupportModuleType = []() -> PyTypeObject*
	{
		static PyTypeObject result = PyTypeObjectInitialized();
		result.tp_name = "tvagentapi.InstantSupportModule";
		result.tp_basicsize = sizeof(PyInstantSupportModule);
		result.tp_dealloc =
			reinterpret_cast<destructor>(DeallocWrapperObject<PyInstantSupportModule>);
		result.tp_flags = Py_TPFLAGS_DEFAULT;
		result.tp_doc = "InstantSupport module class";
		result.tp_methods = PyInstantSupportModule_methods;

		result.tp_dict = PyDict_New();
		PyDict_SetItemString(
			result.tp_dict,
			"SessionState",
			reinterpret_cast<PyObject*>(GetPyTypeInstantSupportModule_SessionState()));
		PyDict_SetItemString(
			result.tp_dict,
			"RequestErrorCode",
			reinterpret_cast<PyObject*>(GetPyTypeInstantSupportModule_RequestErrorCode()));

		if (PyType_Ready(&result) < 0)
		{
			return nullptr;
		}

		return &result;
	}();
	return pyInstantSupportModuleType;
}

PyTypeObject* GetPyTypeInstantSupportModule_SessionState()
{
	static PyTypeObject* pyTypeInstantSupportModule_Status = []() -> PyTypeObject*
	{
		using SessionState = tvagentapi::IInstantSupportModule::SessionState;
		using tvagentapi::toCString;

		PyTypeObject* result =
			CreateEnumType(
				"tvagentapi.InstantSupportModule.SessionState",
				{{toCString(SessionState::Undefined), static_cast<long>(SessionState::Undefined)},
				 {toCString(SessionState::Open), static_cast<long>(SessionState::Open)},
				 {toCString(SessionState::Closed), static_cast<long>(SessionState::Closed)}});

		return result;
	}();
	return pyTypeInstantSupportModule_Status;
}

PyTypeObject* GetPyTypeInstantSupportModule_RequestErrorCode()
{
	static PyTypeObject* pyTypeInstantSupportModule_ErrorCode = []() -> PyTypeObject*
	{
		using ErrorCode = tvagentapi::IInstantSupportModule::RequestErrorCode;
		using tvagentapi::toCString;

		PyTypeObject* result =
			CreateEnumType(
				"tvagentapi.InstantSupportModule.RequestErrorCode",
				{{toCString(ErrorCode::InternalError), static_cast<long>(ErrorCode::InternalError)},
				 {toCString(ErrorCode::InvalidToken), static_cast<long>(ErrorCode::InvalidToken)},
				 {toCString(ErrorCode::InvalidGroup), static_cast<long>(ErrorCode::InvalidGroup)},
				 {toCString(ErrorCode::InvalidSessionCode), static_cast<long>(ErrorCode::InvalidSessionCode)},
				 {toCString(ErrorCode::Busy), static_cast<long>(ErrorCode::Busy)},
				 {toCString(ErrorCode::InvalidEmail), static_cast<long>(ErrorCode::InvalidEmail)},
				 {toCString(ErrorCode::CloseRequestFailed), static_cast<long>(ErrorCode::CloseRequestFailed)},
				 {toCString(ErrorCode::NotFound), static_cast<long>(ErrorCode::NotFound)}});

		return result;
	}();
	return pyTypeInstantSupportModule_ErrorCode;
}

} // namespace tvagentapipy
