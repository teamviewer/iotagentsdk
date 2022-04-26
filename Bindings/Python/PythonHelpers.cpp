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
#include "PythonHelpers.h"

namespace tvagentapipy
{

const char* const InternalError = "Internal call has failed";

PyObject* NoneOrInternalError(bool success)
{
	if (success)
	{
		Py_RETURN_NONE;
	}
	PyErr_SetString(PyExc_RuntimeError, InternalError);
	return nullptr;
}

PyTypeObject* CreateEnumType(
	const std::string& enumName,
	const std::map<std::string, long>& enumValues)
{
	PyObject* pyModuleEnum = PyImport_ImportModule("enum");
	if (pyModuleEnum == nullptr)
	{
		PyErr_SetString(PyExc_ImportError, "enum");
		return nullptr;
	}

	PyObject* pyTypeEnum = PyObject_GetAttrString(pyModuleEnum, "Enum");
	if (pyTypeEnum == nullptr)
	{
		Py_XDECREF(pyModuleEnum);
		PyErr_SetString(PyExc_ImportError, "enum.Enum");
		return nullptr;
	}

	PyObject* pyAttrs = PyDict_New();
	for (const auto& enumValue : enumValues)
	{
		PyObject* pyValue = PyLong_FromLong(enumValue.second);
		PyDict_SetItemString(pyAttrs, enumValue.first.c_str(), pyValue);
		Py_XDECREF(pyValue);
	}

	PyObject* pyArgs = Py_BuildValue("sO", enumName.c_str(), pyAttrs);
	Py_XDECREF(pyAttrs);

	PyObject* pyNewEnumType = PyObject_CallObject(pyTypeEnum, pyArgs);

	Py_XDECREF(pyArgs);
	Py_XDECREF(pyTypeEnum);
	Py_XDECREF(pyModuleEnum);

	return reinterpret_cast<PyTypeObject*>(pyNewEnumType);
}

PyObject* PyEnumValue(
	PyTypeObject* pyEnumType,
	const std::string& enumValueName)
{
	PyObject* pyEnumValue = PyObject_GetAttrString(
		reinterpret_cast<PyObject*>(pyEnumType),
		enumValueName.c_str());
	return pyEnumValue;
}

PyTypeObject PyTypeObjectInitialized()
{
	// We'r not able to use designated initializers (available only in C99 and C++20) to
	// partially initialize PyTypeObject with PyVarObject_HEAD_INIT macro and have the
	// rest of it being zero-initialized. Partially initialized PyTypeObject causes a
	// compilation warning, therefore we use this helper function with initalization via
	// Head struct, which replicates PyTypeObject header fields.
	struct Head
	{
		PyObject_VAR_HEAD
	};

	static const Head head = {
		PyVarObject_HEAD_INIT(nullptr, 0)
	};

	PyTypeObject typeObject{};
	typeObject.ob_base = head.ob_base;
	return typeObject;
}

} // namespace tvagentapipy
