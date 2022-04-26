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
#pragma once

#include "Typesystem.h"

#include <Python.h>

#include <map>
#include <string>

namespace tvagentapipy
{

extern const char* const InternalError;

PyObject* NoneOrInternalError(bool success);

PyTypeObject* CreateEnumType(
	const std::string& enumName,
	const std::map<std::string, long>& enumValues);

// Returns new reference
PyObject* PyEnumValue(PyTypeObject* pyEnumType, const std::string& enumValueName);

// On internal error sets PyErr which can be checked with PyErr_Occurred()
template<typename EnumType>
EnumType EnumFromPyEnumValue(PyObject* enumValue)
{
	PyTypeObject* type = GetPyTypeObject<EnumType>();
	if (type != Py_TYPE(enumValue))
	{
		PyErr_Format(PyExc_TypeError, "expected %R, got %R", type, Py_TYPE(enumValue));
		return {};
	}

	PyObject* pyLong = PyObject_GetAttrString(enumValue, "value");
	if (!pyLong)
	{
		// PyObject_GetAttrString sets PyErr
		return {};
	}

	if (!PyLong_Check(pyLong))
	{
		Py_DECREF(pyLong);
		PyErr_SetString(PyExc_TypeError, "value is not of type long");
		return {};
	}

	EnumType ret = static_cast<EnumType>(PyLong_AsLong(pyLong));
	Py_DECREF(pyLong);
	return ret;
}

// Returns PyTypeObject with header initialized with PyVarObject_HEAD_INIT and all the
// other fields default initialized to 0
PyTypeObject PyTypeObjectInitialized();

} // namespace tvagentapipy
