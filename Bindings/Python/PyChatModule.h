#pragma once

#include "prototypes.h"
#include "PyWeakObject.h"

#include <TVAgentAPI/prototypes.h>

#include <Python.h>

namespace tvagentapipy
{

struct PyChatModule final
{
	PyObject_HEAD

	PyChatModule(PyAgentConnection* pyAgentConnection);
	~PyChatModule();

	bool IsReady() const;

	PyWeakObject m_pyWeakAgentConnection{};
	tvagentapi::IChatModule* m_module = nullptr;

	PyObject* m_pyChatCreatedCallback = nullptr;
	PyObject* m_pyChatsRemovedCallback = nullptr;
	PyObject* m_pyChatClosedCallback = nullptr;
	PyObject* m_pyReceivedMessagesCallback = nullptr;
	PyObject* m_pySendMessageFinishedCallback = nullptr;
	PyObject* m_pyMessagesLoadedCallback = nullptr;
	PyObject* m_pyHistoryDeletedCallback = nullptr;
};

PyTypeObject* GetPyTypeChatModule();
PyTypeObject* GetPyTypeChatModule_ChatEndpointType();
PyTypeObject* GetPyTypeChatModule_ChatState();
PyTypeObject* GetPyTypeChatModule_SendMessageResult();

} // namespace tvagentapipy
