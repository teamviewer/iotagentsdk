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

#include <Python.h>

#include <utility>

namespace tvagentapipy
{

struct PyTypeMeta final
{
	PyTypeObject* pyTypeObject = nullptr;
	const char* publicName = nullptr;
};

template <typename WrapperType>
PyTypeMeta GetPyTypeMeta();

template <typename WrapperType>
PyTypeObject* GetPyTypeObject()
{
	return GetPyTypeMeta<WrapperType>().pyTypeObject;
}

template <typename WrapperType>
void DeallocWrapperObject(WrapperType* self);

template <typename WrapperType, typename... Args>
WrapperType* MakeWrapperObject(Args&&... args)
{
	PyTypeObject* pyTypeObject = GetPyTypeObject<WrapperType>();
	auto pyWrapper =
		reinterpret_cast<WrapperType*>(pyTypeObject->tp_alloc(pyTypeObject, 0));
	if (!pyWrapper)
	{
		PyErr_Format(
			PyExc_MemoryError,
			"Failed to allocate memory for %s instance",
			pyTypeObject->tp_name);
		return nullptr;
	}

	new(pyWrapper) WrapperType(std::forward<Args>(args)...);

	if (!pyWrapper->IsReady() || PyErr_Occurred())
	{
		DeallocWrapperObject(pyWrapper);
		if (!PyErr_Occurred())
		{
			PyErr_Format(
				PyExc_RuntimeError,
				"Failed to create an instance of %s",
				pyTypeObject->tp_name);
		}
		return nullptr;
	}
	return pyWrapper;
}

template <typename WrapperType, PyObject* (*Method)(WrapperType*, PyObject*)>
PyObject* WeakConnectionCall(PyObject* pySelf, PyObject* args)
{
	auto self = reinterpret_cast<WrapperType*>(pySelf);
	if (!self->m_pyWeakAgentConnection)
	{
		PyErr_Format(PyExc_ReferenceError, "Agent connection is lost for %R", pySelf);
		return nullptr;
	}

	return Method(self, args);
}

template <typename WrapperType, PyObject* (*Method)(WrapperType*, PyObject*, PyObject*)>
PyObject* WeakConnectionCall(PyObject* pySelf, PyObject* args, PyObject* kwargs)
{
	auto self = reinterpret_cast<WrapperType*>(pySelf);
	if (!self->m_pyWeakAgentConnection)
	{
		PyErr_Format(PyExc_ReferenceError, "Agent connection is lost for %R", pySelf);
		return nullptr;
	}

	return Method(self, args, kwargs);
}

template <typename WrapperType>
void DeallocWrapperObject(WrapperType* self)
{
	self->~WrapperType();
	Py_TYPE(self)->tp_free(self);
}

} // namespace tvagentapipy
