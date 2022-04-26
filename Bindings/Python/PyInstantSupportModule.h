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

#include "prototypes.h"
#include "PyWeakObject.h"

#include <TVAgentAPI/prototypes.h>

#include <Python.h>

namespace tvagentapipy
{

struct PyInstantSupportModule final
{
	PyObject_HEAD

	PyInstantSupportModule(PyAgentConnection* pyAgentConnection);
	~PyInstantSupportModule();

	bool IsReady() const;

	PyWeakObject m_pyWeakAgentConnection{};
	tvagentapi::IInstantSupportModule* m_module = nullptr;
	PyObject* m_pySessionDataChangedCallback = nullptr;
	PyObject* m_pyRequestErrorCallback = nullptr;
	PyObject* m_pyConnectionRequestCallback = nullptr;
};

PyTypeObject* GetPyTypeInstantSupportModule();
PyTypeObject* GetPyTypeInstantSupportModule_SessionState();
PyTypeObject* GetPyTypeInstantSupportModule_RequestErrorCode();

} // namespace tvagentapipy
