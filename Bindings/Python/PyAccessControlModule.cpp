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

#include <TVAgentAPI/AccessControlModuleStringify.h>
#include <TVAgentAPI/IAccessControlModule.h>
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

PyObject* PyAccessControlModule_setCallbacks(PyAccessControlModule* self, PyObject* arg)
{
	using Feature = tvagentapi::IAccessControlModule::Feature;
	using Access = tvagentapi::IAccessControlModule::Access;

	if (!PyDict_Check(arg))
	{
		PyErr_Format(PyExc_TypeError, "expected dict instance, got %R", arg);
		return nullptr;
	}

	const char* accessChangedCbkName = "accessChangedCallback";
	const char* accessRequestCbName = "accessRequestCallback";

	PyObject* accessChangedCallback = PyDict_GetItemString(arg, accessChangedCbkName);
	PyObject* accessRequestCallback = PyDict_GetItemString(arg, accessRequestCbName);

	if (accessChangedCallback && !PyCallable_Check(accessChangedCallback))
	{
		PyErr_Format(PyExc_TypeError, "%R is not callable", accessChangedCallback);
		return nullptr;
	}

	if (accessRequestCallback && !PyCallable_Check(accessRequestCallback))
	{
		PyErr_Format(PyExc_TypeError, "%R is not callable", accessRequestCallback);
		return nullptr;
	}

	Py_XINCREF(accessChangedCallback);
	Py_XINCREF(accessRequestCallback);
	Py_XDECREF(self->m_pyAccessChangedCallback);
	Py_XDECREF(self->m_pyAccessRequestCallback);
	self->m_pyAccessChangedCallback = accessChangedCallback;
	self->m_pyAccessRequestCallback = accessRequestCallback;

	auto accessChanged = [](Feature feature, Access access, void* userdata) noexcept
	{
		auto pyAccessChangedCallback = static_cast<PyObject*>(userdata);

		PyObject* featureEnumValue = PyEnumValue(
			GetPyTypeAccessControlModule_Feature(),
			tvagentapi::toCString(feature));
		PyObject* accessEnumValue = PyEnumValue(
			GetPyTypeAccessControlModule_Access(),
			tvagentapi::toCString(access));

		PyObject* args = Py_BuildValue("(OO)", featureEnumValue, accessEnumValue);

		PyObject* result = PyObject_CallObject(pyAccessChangedCallback, args);
		Py_XDECREF(result);
		Py_DECREF(args);
		Py_DECREF(featureEnumValue);
		Py_DECREF(accessEnumValue);
	};

	auto accessRequest = [](Feature feature, void* userdata) noexcept
	{
		auto pyAccessConfirmationCallback = static_cast<PyObject*>(userdata);

		PyObject* enumVal = PyEnumValue(
			GetPyTypeAccessControlModule_Feature(),
			tvagentapi::toCString(feature));

		// TODO When Python >= 3.9 use PyObject_CallOneArg
		PyObject* args = Py_BuildValue("(O)", enumVal);

		PyObject* result = PyObject_CallObject(pyAccessConfirmationCallback, args);
		Py_DECREF(args);
		Py_XDECREF(result);
		Py_DECREF(enumVal);
	};

	tvagentapi::IAccessControlModule::AccessChangedCallback accessChangedCb{};
	if (self->m_pyAccessChangedCallback)
	{
		accessChangedCb = {accessChanged, self->m_pyAccessChangedCallback};
	}

	tvagentapi::IAccessControlModule::AccessConfirmationRequestCallback accessRequestCb{};
	if (self->m_pyAccessRequestCallback)
	{
		accessRequestCb = {accessRequest, self->m_pyAccessRequestCallback};
	}

	self->m_module->setCallbacks({accessChangedCb, accessRequestCb});

	Py_RETURN_NONE;
}

PyObject* PyAccessControlModule_getAccess(PyAccessControlModule* self, PyObject* arg)
{
	using Feature = tvagentapi::IAccessControlModule::Feature;
	using Access = tvagentapi::IAccessControlModule::Access;

	Feature feature = EnumFromPyEnumValue<Feature>(arg);
	if (PyErr_Occurred())
	{
		return nullptr;
	}

	Access access;
	if (!self->m_module->getAccess(feature, access))
	{
		PyErr_SetString(PyExc_RuntimeError, InternalError);
		return nullptr;
	}

	return PyEnumValue(
		GetPyTypeAccessControlModule_Access(),
		tvagentapi::toCString(access));
}

PyObject* PyAccessControlModule_setAccess(PyAccessControlModule* self, PyObject* args)
{
	using Feature = tvagentapi::IAccessControlModule::Feature;
	using Access = tvagentapi::IAccessControlModule::Access;

	PyObject* featureEnum;
	PyObject* accessEnum;
	if (!PyArg_ParseTuple(args, "OO", &featureEnum, &accessEnum))
	{
		return nullptr;
	}

	Feature feature = EnumFromPyEnumValue<Feature>(featureEnum);
	if (PyErr_Occurred())
	{
		return nullptr;
	}

	Access access = EnumFromPyEnumValue<Access>(accessEnum);
	if (PyErr_Occurred())
	{
		return nullptr;
	}

	return NoneOrInternalError(self->m_module->setAccess(feature, access));
}

PyObject* PyAccessControlModule_isSupported(PyAccessControlModule* self, PyObject* args)
{
	(void)args;
	if (self->m_module->isSupported())
	{
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

PyObject* PyAccessControlModule_acceptAccessRequest(PyAccessControlModule* self, PyObject* arg)
{
	using Feature = tvagentapi::IAccessControlModule::Feature;

	Feature feature = EnumFromPyEnumValue<Feature>(arg);
	if (PyErr_Occurred())
	{
		return nullptr;
	}
	return NoneOrInternalError(self->m_module->acceptAccessRequest(feature));
}

PyObject* PyAccessControlModule_rejectAccessRequest(PyAccessControlModule* self, PyObject* arg)
{
	using Feature = tvagentapi::IAccessControlModule::Feature;

	Feature feature = EnumFromPyEnumValue<Feature>(arg);
	if (PyErr_Occurred())
	{
		return nullptr;
	}
	return NoneOrInternalError(self->m_module->rejectAccessRequest(feature));
}

PyMethodDef PyAccessControlModule_methods[] =
{
	{
		"setCallbacks",
		WeakConnectionCall<PyAccessControlModule, PyAccessControlModule_setCallbacks>,
		METH_O,
		"set access control callbacks"
	},

	{
		"getAccess",
		WeakConnectionCall<PyAccessControlModule, PyAccessControlModule_getAccess>,
		METH_O,
		"for given Feature fetch Access mode"
	},

	{
		"setAccess",
		WeakConnectionCall<PyAccessControlModule, PyAccessControlModule_setAccess>,
		METH_VARARGS,
		"for given Feature set Access mode"
	},

	{
		"isSupported",
		WeakConnectionCall<PyAccessControlModule, PyAccessControlModule_isSupported>,
		METH_NOARGS,
		"check if the module is supported"
	},

	{
		"acceptAccessRequest",
		WeakConnectionCall<PyAccessControlModule, PyAccessControlModule_acceptAccessRequest>,
		METH_O,
		"for given Feature accept incoming access request"
	},

	{
		"rejectAccessRequest",
		WeakConnectionCall<PyAccessControlModule, PyAccessControlModule_rejectAccessRequest>,
		METH_O,
		"for given Feature reject incoming access request"
	},

	{} // Sentinel
};

} // namespace

namespace tvagentapipy
{

PyAccessControlModule::PyAccessControlModule(PyAgentConnection* pyAgentConnection)
	: m_pyWeakAgentConnection{reinterpret_cast<PyObject*>(pyAgentConnection)}
{
	m_module = static_cast<tvagentapi::IAccessControlModule*>(
		pyAgentConnection->m_connection->getModule(
			tvagentapi::IModule::Type::AccessControl
		));
}

PyAccessControlModule::~PyAccessControlModule()
{
	Py_XDECREF(m_pyAccessChangedCallback);
	Py_XDECREF(m_pyAccessRequestCallback);
}

bool PyAccessControlModule::IsReady() const
{
	return !!m_module;
}

PyTypeObject* GetPyTypeAccessControlModule()
{
	static PyTypeObject* pyAccessControlModuleType = []() -> PyTypeObject*
	{
		static PyTypeObject result = PyTypeObjectInitialized();
		result.tp_name = "tvagentapi.AccessControlModule";
		result.tp_basicsize = sizeof(PyAccessControlModule);
		result.tp_dealloc =
			reinterpret_cast<destructor>(DeallocWrapperObject<PyAccessControlModule>);
		result.tp_flags = Py_TPFLAGS_DEFAULT;
		result.tp_doc = "AccessControl module class";
		result.tp_methods = PyAccessControlModule_methods;

		result.tp_dict = PyDict_New();
		PyDict_SetItemString(
			result.tp_dict,
			"Feature",
			reinterpret_cast<PyObject*>(GetPyTypeAccessControlModule_Feature()));
		PyDict_SetItemString(
			result.tp_dict,
			"Access",
			reinterpret_cast<PyObject*>(GetPyTypeAccessControlModule_Access()));

		if (PyType_Ready(&result) < 0)
		{
			return nullptr;
		}

		return &result;
	}();
	return pyAccessControlModuleType;
}

PyTypeObject* GetPyTypeAccessControlModule_Feature()
{
	static PyTypeObject* pyTypeAccessControlModule_Feature = []() -> PyTypeObject*
	{
		using Feature = tvagentapi::IAccessControlModule::Feature;
		using tvagentapi::toCString;

		PyTypeObject* result =
			CreateEnumType(
				"tvagentapi.AccessControlModule.Feature",
				{{toCString(Feature::FileTransfer), static_cast<long>(Feature::FileTransfer)},
				 {toCString(Feature::RemoteView), static_cast<long>(Feature::RemoteView)},
				 {toCString(Feature::RemoteControl), static_cast<long>(Feature::RemoteControl)}});

		return result;
	}();
	return pyTypeAccessControlModule_Feature;
}

PyTypeObject* GetPyTypeAccessControlModule_Access()
{
	static PyTypeObject* pyTypeAccessControlModule_Access = []() -> PyTypeObject*
	{
		using Access = tvagentapi::IAccessControlModule::Access;
		using tvagentapi::toCString;

		PyTypeObject* result =
			CreateEnumType(
				"tvagentapi.AccessControlModule.Access",
				{{toCString(Access::Allowed), static_cast<long>(Access::Allowed)},
				 {toCString(Access::AfterConfirmation), static_cast<long>(Access::AfterConfirmation)},
				 {toCString(Access::Denied), static_cast<long>(Access::Denied)}});

		return result;
	}();
	return pyTypeAccessControlModule_Access;
}

} // namespace tvagentapipy
