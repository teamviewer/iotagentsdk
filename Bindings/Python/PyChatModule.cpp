#include "PyChatModule.h"

#include <TVAgentAPI/ChatModuleStringify.h>
#include <TVAgentAPI/IChatModule.h>
#include <TVAgentAPI/IAgentAPI.h>
#include <TVAgentAPI/IAgentConnection.h>

#include "PyAgentConnection.h"
#include "PyTVAgentAPI.h"
#include "PythonHelpers.h"
#include "Typesystem.h"

#include <assert.h>
#include <iostream>

using namespace tvagentapipy;

namespace
{

// Methods

PyObject* CreateMessageDict(const tvagentapi::IChatModule::ChatMessage* message)
{
	if (!message)
	{
		return nullptr;
	}

	PyObject* pyChatID    = PyUnicode_FromString(message->chatID);
	PyObject* pyMessageID = PyUnicode_FromString(message->messageID);
	PyObject* pyContent   = PyUnicode_FromString(message->content);
	PyObject* pyTimeStamp = PyLong_FromLong(message->timeStamp);
	PyObject* pySender    = PyUnicode_FromString(message->sender);

	PyObject* pyMessage = PyDict_New();
	assert(pyMessage);

	PyDict_SetItemString(pyMessage, "chatID", pyChatID);
	PyDict_SetItemString(pyMessage, "messageID", pyMessageID);
	PyDict_SetItemString(pyMessage, "content", pyContent);
	PyDict_SetItemString(pyMessage, "timeStamp", pyTimeStamp);
	PyDict_SetItemString(pyMessage, "sender", pySender);
	Py_DECREF(pyChatID);
	Py_DECREF(pyMessageID);
	Py_DECREF(pyContent);
	Py_DECREF(pyTimeStamp);
	Py_DECREF(pySender);

	return pyMessage;
}

PyObject* PyChatModule_setCallbacks(
	PyChatModule* self,
	PyObject* args,
	PyObject* kwargs)
{
	char chatCreatedCallbackArgName[] = "chatCreatedCallback";
	char chatsRemovedCallbackArgName[] = "chatsRemovedCallback";
	char chatClosedCallbackArgName[] = "chatClosedCallback";
	char receivedMessagesCallbackArgName[] = "receivedMessagesCallback";
	char sendMessageFinishedCallbackArgName[] = "sendMessageFinishedCallback";
	char messagesLoadedCallbackArgName[] = "messagesLoadedCallback";
	char historyDeletedCallbackArgName[] = "historyDeletedCallback";

	PyObject* chatCreatedCallback         = Py_None;
	PyObject* chatsRemovedCallback        = Py_None;
	PyObject* chatClosedCallback          = Py_None;
	PyObject* receivedMessagesCallback    = Py_None;
	PyObject* sendMessageFinishedCallback = Py_None;
	PyObject* messagesLoadedCallback      = Py_None;
	PyObject* historyDeletedCallback      = Py_None;

	char* kwargList[] = {
		chatCreatedCallbackArgName,
		chatsRemovedCallbackArgName,
		chatClosedCallbackArgName,
		receivedMessagesCallbackArgName,
		sendMessageFinishedCallbackArgName,
		messagesLoadedCallbackArgName,
		historyDeletedCallbackArgName,
		{}};
	if (!PyArg_ParseTupleAndKeywords(
		args,
		kwargs,
		"|OOOOOOO:setCallbacks",
		kwargList,
		&chatCreatedCallback,
		&chatsRemovedCallback,
		&chatClosedCallback,
		&receivedMessagesCallback,
		&sendMessageFinishedCallback,
		&messagesLoadedCallback,
		&historyDeletedCallback))
	{
		return nullptr;
	}

	chatCreatedCallback         = chatCreatedCallback         == Py_None ? nullptr : chatCreatedCallback;
	chatsRemovedCallback        = chatsRemovedCallback        == Py_None ? nullptr : chatsRemovedCallback;
	chatClosedCallback          = chatClosedCallback          == Py_None ? nullptr : chatClosedCallback;
	receivedMessagesCallback    = receivedMessagesCallback    == Py_None ? nullptr : receivedMessagesCallback;
	sendMessageFinishedCallback = sendMessageFinishedCallback == Py_None ? nullptr : sendMessageFinishedCallback;
	messagesLoadedCallback      = messagesLoadedCallback      == Py_None ? nullptr : messagesLoadedCallback;
	historyDeletedCallback      = historyDeletedCallback      == Py_None ? nullptr : historyDeletedCallback;

	if (chatCreatedCallback && !PyCallable_Check(chatCreatedCallback))
	{
		PyErr_Format(PyExc_TypeError, "%R is not callable", chatCreatedCallback);
		return nullptr;
	}

	if (chatsRemovedCallback && !PyCallable_Check(chatsRemovedCallback))
	{
		PyErr_Format(PyExc_TypeError, "%R is not callable", chatsRemovedCallback);
		return nullptr;
	}

	if (chatClosedCallback && !PyCallable_Check(chatClosedCallback))
	{
		PyErr_Format(PyExc_TypeError, "%R is not callable", chatClosedCallback);
		return nullptr;
	}

	if (receivedMessagesCallback && !PyCallable_Check(receivedMessagesCallback))
	{
		PyErr_Format(PyExc_TypeError, "%R is not callable", receivedMessagesCallback);
		return nullptr;
	}

	if (sendMessageFinishedCallback && !PyCallable_Check(sendMessageFinishedCallback))
	{
		PyErr_Format(PyExc_TypeError, "%R is not callable", sendMessageFinishedCallback);
		return nullptr;
	}

	if (messagesLoadedCallback && !PyCallable_Check(messagesLoadedCallback))
	{
		PyErr_Format(PyExc_TypeError, "%R is not callable", messagesLoadedCallback);
		return nullptr;
	}

	if (historyDeletedCallback && !PyCallable_Check(historyDeletedCallback))
	{
		PyErr_Format(PyExc_TypeError, "%R is not callable", historyDeletedCallback);
		return nullptr;
	}

	Py_XINCREF(chatCreatedCallback);
	Py_XDECREF(self->m_pyChatCreatedCallback);
	self->m_pyChatCreatedCallback = chatCreatedCallback;

	Py_XINCREF(chatsRemovedCallback);
	Py_XDECREF(self->m_pyChatsRemovedCallback);
	self->m_pyChatsRemovedCallback = chatsRemovedCallback;

	Py_XINCREF(chatClosedCallback);
	Py_XDECREF(self->m_pyChatClosedCallback);
	self->m_pyChatClosedCallback = chatClosedCallback;

	Py_XINCREF(receivedMessagesCallback);
	Py_XDECREF(self->m_pyReceivedMessagesCallback);
	self->m_pyReceivedMessagesCallback = receivedMessagesCallback;

	Py_XINCREF(sendMessageFinishedCallback);
	Py_XDECREF(self->m_pySendMessageFinishedCallback);
	self->m_pySendMessageFinishedCallback = sendMessageFinishedCallback;

	Py_XINCREF(messagesLoadedCallback);
	Py_XDECREF(self->m_pyMessagesLoadedCallback);
	self->m_pyMessagesLoadedCallback = messagesLoadedCallback;

	Py_XINCREF(historyDeletedCallback);
	Py_XDECREF(self->m_pyHistoryDeletedCallback);
	self->m_pyHistoryDeletedCallback = historyDeletedCallback;

	auto chatCreated = [](
		const char* chatId,
		const char* title,
		tvagentapi::IChatModule::ChatEndpointType chatEndpointType,
		uint64_t chatTypeId,
		void* userdata)
	{
	 	auto pyChatCreatedCallback = static_cast<PyObject*>(userdata);

		PyObject* chatEndpointTypeEnumVal = PyEnumValue(
			GetPyTypeChatModule_ChatEndpointType(),
			tvagentapi::toCString(chatEndpointType));

	 	PyObject* args = Py_BuildValue("(ssOl)", chatId, title, chatEndpointTypeEnumVal, chatTypeId);

		PyObject* result = PyObject_CallObject(pyChatCreatedCallback, args);
		Py_DECREF(args);
		Py_XDECREF(result);
		Py_DECREF(chatEndpointTypeEnumVal);
	};

	auto chatsRemoved = [](
		const char** chatIDs,
		size_t chatIDsCount,
		void* userdata)
	{
		auto pyChatRemovedCallback = static_cast<PyObject*>(userdata);

		PyObject* chadIDsList = PyList_New(chatIDsCount);
		assert(chadIDsList);
		for (size_t i = 0; i < chatIDsCount; i++)
		{
			PyObject* chatID = PyUnicode_FromString(chatIDs[i]);
			assert(chatID);
			PyList_SET_ITEM(chadIDsList, i, chatID);
		}

		PyObject* args = Py_BuildValue("(O)", chadIDsList);

		PyObject* result = PyObject_CallObject(pyChatRemovedCallback, args);
		Py_DECREF(args);
		Py_XDECREF(result);
	};

	auto chatClosed = [](
		const char* chatID,
		void* userdata)
	{
		auto pyChatClosedCallback = static_cast<PyObject*>(userdata);

		PyObject* args = Py_BuildValue("(s)", chatID);

		PyObject* result = PyObject_CallObject(pyChatClosedCallback, args);
		Py_DECREF(args);
		Py_XDECREF(result);
	};

	auto receivedMessages = [](
		const tvagentapi::IChatModule::ChatMessage* messages,
		size_t messagesCount,
		void* userdata)
	{
		auto receivedMessagesCallback = static_cast<PyObject*>(userdata);

		PyObject* messagesList = PyList_New(messagesCount);
		assert(messagesList);
		for (size_t i = 0; i < messagesCount; i++)
		{
			PyObject* message = CreateMessageDict(&messages[i]);
			PyList_SET_ITEM(messagesList, i, message);
		}

		PyObject* args = Py_BuildValue("(O)", messagesList);

		PyObject* result = PyObject_CallObject(receivedMessagesCallback, args);
		Py_DECREF(args);
		Py_XDECREF(result);
		Py_DECREF(messagesList);
	};

	auto sendMessageFinished = [](
		tvagentapi::IChatModule::RequestID requestID,
		tvagentapi::IChatModule::SendMessageResult sendMessageResult,
		const char* messageID,
		tvagentapi::IChatModule::TimeStamp timeStamp,
		void* userdata)
	{
		auto pySendMessageFinishedCallback = static_cast<PyObject*>(userdata);

		PyObject* sendMessageResultEnumVal = PyEnumValue(
			GetPyTypeChatModule_SendMessageResult(),
			tvagentapi::toCString(sendMessageResult));

		PyObject* args = Py_BuildValue("(IOsk)", requestID, sendMessageResultEnumVal, messageID, timeStamp);

		PyObject* result = PyObject_CallObject(pySendMessageFinishedCallback, args);
		Py_DECREF(args);
		Py_XDECREF(result);
		Py_XDECREF(sendMessageResultEnumVal);
	};

	auto messagesLoaded = [](
		const tvagentapi::IChatModule::ChatMessage* messages,
		size_t messagesCount,
		bool hasMore,
		void* userdata)
	{
		auto messagesLoadedCallback = static_cast<PyObject*>(userdata);

		PyObject* messagesList = PyList_New(messagesCount);
		assert(messagesList);
		for (size_t i = 0; i < messagesCount; i++)
		{
			PyObject* message = CreateMessageDict(&messages[i]);
			PyList_SET_ITEM(messagesList, i, message);
		}

		PyObject* args = Py_BuildValue("(OO)", messagesList, hasMore ? Py_True : Py_False);

		PyObject* result = PyObject_CallObject(messagesLoadedCallback, args);
		Py_DECREF(args);
		Py_XDECREF(result);
		Py_DECREF(messagesList);
	};

	auto historyDeleted = [](
		const char* chatID,
		void* userdata)
	{
		auto historyDeletedCallback = static_cast<PyObject*>(userdata);

		PyObject* args = Py_BuildValue("(s)", chatID);

		PyObject* result = PyObject_CallObject(historyDeletedCallback, args);
		Py_DECREF(args);
		Py_XDECREF(result);
	};

	tvagentapi::IChatModule::ChatCreatedCallback chatCreatedCb{};
	if (self->m_pyChatCreatedCallback)
	{
		chatCreatedCb = {chatCreated, self->m_pyChatCreatedCallback};
	}

	tvagentapi::IChatModule::ChatsRemovedCallback chatsRemovedCb{};
	if (self->m_pyChatsRemovedCallback)
	{
		chatsRemovedCb = {chatsRemoved, self->m_pyChatsRemovedCallback};
	}

	tvagentapi::IChatModule::ChatClosedCallback chatClosedCb{};
	if (self->m_pyChatClosedCallback)
	{
		chatClosedCb = {chatClosed, self->m_pyChatClosedCallback};
	}

	tvagentapi::IChatModule::ReceivedMessagesCallback receivedMessagesCb{};
	if (self->m_pyReceivedMessagesCallback)
	{
		receivedMessagesCb = {receivedMessages, self->m_pyReceivedMessagesCallback};
	}

	tvagentapi::IChatModule::SendMessageFinishedCallback sendMessageFinishedCb{};
	if (self->m_pySendMessageFinishedCallback)
	{
		sendMessageFinishedCb = {sendMessageFinished, self->m_pySendMessageFinishedCallback};
	}

	tvagentapi::IChatModule::MessagesLoadedCallback messagesLoadedCb{};
	if (self->m_pyMessagesLoadedCallback)
	{
		messagesLoadedCb = {messagesLoaded, self->m_pyMessagesLoadedCallback};
	}

	tvagentapi::IChatModule::HistoryDeletedCallback historyDeletedCb{};
	if (self->m_pyHistoryDeletedCallback)
	{
		historyDeletedCb = {historyDeleted, self->m_pyHistoryDeletedCallback};
	}

	self->m_module->setCallbacks({
		chatCreatedCb,
		chatsRemovedCb,
		chatClosedCb,
		receivedMessagesCb,
		sendMessageFinishedCb,
		messagesLoadedCb,
		historyDeletedCb
	});

	Py_RETURN_NONE;
}

PyObject* PyChatModule_obtainChats(PyChatModule* self, PyObject* args)
{
	(void)args;

	auto enumerator = [](
		const char* chatID,
		const char* title,
		tvagentapi::IChatModule::ChatEndpointType chatEndpointType,
		uint64_t chatEndpointID,
		tvagentapi::IChatModule::ChatState chatState,
		void* userdata) -> bool
	{
		auto pyList = static_cast<PyObject*>(userdata);

		PyObject* pyChatID           = PyUnicode_FromString(chatID);
		PyObject* pyTitle            = PyUnicode_FromString(title);
		PyObject* pyChatEndpointType = PyEnumValue(GetPyTypeChatModule_ChatEndpointType(), toCString(chatEndpointType));
		PyObject* pyChatEndpointID   = PyLong_FromUnsignedLongLong(chatEndpointID);
		PyObject* pyChatState        = PyEnumValue(GetPyTypeChatModule_ChatState(), toCString(chatState));

		PyObject* pyChatInfo = PyDict_New();
		PyDict_SetItemString(pyChatInfo, "chatID",           pyChatID);
		PyDict_SetItemString(pyChatInfo, "title",            pyTitle);
		PyDict_SetItemString(pyChatInfo, "chatEndpointType", pyChatEndpointType);
		PyDict_SetItemString(pyChatInfo, "chatEndpointID",   pyChatEndpointID);
		PyDict_SetItemString(pyChatInfo, "chatState",        pyChatState);

		Py_DECREF(pyChatID);
		Py_DECREF(pyTitle);
		Py_DECREF(pyChatEndpointType);
		Py_DECREF(pyChatEndpointID);
		Py_DECREF(pyChatState);

		PyList_Append(pyList, pyChatInfo);
		Py_DECREF(pyChatInfo);

		return true;
	};

	PyObject* pyList = PyList_New(/*size=*/0);

	if (!self->m_module->obtainChats({enumerator, pyList}))
	{
		Py_DECREF(pyList);
		PyErr_SetString(PyExc_RuntimeError, "Failed to obtain chats");
		return nullptr;
	}

	return pyList;
}

PyObject* PyChatModule_sendMessage(PyChatModule* self, PyObject* args)
{
	const char* chatID = nullptr;
	const char* content = nullptr;
	if (!PyArg_ParseTuple(args, "ss", &chatID, &content))
	{
		return nullptr;
	}

	const tvagentapi::IChatModule::RequestID requestID = self->m_module->sendMessage(
		chatID,
		content);

	if (requestID == tvagentapi::IChatModule::InvalidRequestID)
	{
		PyErr_SetString(PyExc_RuntimeError, "Internal call has failed");
		return nullptr;
	}

	return PyLong_FromUnsignedLong(requestID);
}

PyObject* PyChatModule_loadMessages(PyChatModule* self, PyObject* args)
{
	const char* chatID = nullptr;
	PyObject* pyMessageCount = nullptr;
	const char* messagesBefore = nullptr;
	if (!PyArg_ParseTuple(args, "sO|z", &chatID, &pyMessageCount, &messagesBefore))
	{
		return nullptr;
	}

	const size_t messageCount = PyLong_AsSize_t(pyMessageCount);
	if (PyErr_Occurred())
	{
		return nullptr;
	}

	return NoneOrInternalError(self->m_module->loadMessages(
		chatID,
		messageCount,
		messagesBefore));
}

PyObject* PyChatModule_deleteHistory(PyChatModule* self, PyObject* arg)
{
	if (!PyUnicode_Check(arg))
	{
		PyErr_Format(PyExc_TypeError, "expected string instance, got %R", arg);
		return nullptr;
	}

	return NoneOrInternalError(self->m_module->deleteHistory(PyUnicode_AsUTF8(arg)));
}

PyObject* PyChatModule_deleteChat(PyChatModule* self, PyObject* arg)
{
	if (!PyUnicode_Check(arg))
	{
		PyErr_Format(PyExc_TypeError, "expected string instance, got %R", arg);
		return nullptr;
	}

	return NoneOrInternalError(self->m_module->deleteChat(PyUnicode_AsUTF8(arg)));
}

PyObject* PyChatModule_isSupported(PyChatModule* self, PyObject* args)
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
	chatCreatedCallback=None,
	chatsRemovedCallback=None,
	chatClosedCallback=None,
	receivedMessagesCallback=None,
	sendMessageFinishedCallback=None,
	messagesLoadedCallback=None,
	historyDeletedCallback=None)
--

Sets callbacks to handle various events like chat room and message updates.

:param chatCreatedCallback: called when a new chat room has been created
:type chatCreatedCallback: callback(str chatId, str title, tvagentapi.ChatModule.ChatEndpointType chatEndpointType, int chatTypeId)
:param chatsRemovedCallback: called when a chat room has been deleted
:type chatsRemovedCallback: callback(list chadIDsList)
:param chatClosedCallback: called when a chat room has been closed
:type chatClosedCallback: callback(str chatID)
:param receivedMessagesCallback: called when messages have been received. The given list consists of dictionaries - one for each message - with the keys 'chatID', 'message', 'content', 'timeStamp', 'sender'.
:type receivedMessagesCallback: callback(list messageList)
:param sendMessageFinishedCallback: called when the result for a send request from sendMessage() is available
:type sendMessageFinishedCallback: callback(int requestID, tvagentapi.ChatModule.SendMessageResult sendMessageResult, str messageID, int timeStamp)
:param messagesLoadedCallback: called as response of a loadMessages() call. The given list of messages consists of dictionaries - one for each message - with the keys 'chatID', 'message', 'content', 'timeStamp', 'sender'.
:type messagesLoadedCallback: callback(list messagesList, bool hasMore)
:param historyDeletedCallback: called when the history of a chat room has been deleted
:type historyDeletedCallback: callback(str chatID)
)__");

PyDoc_STRVAR(obtainChats,
R"__(obtainChats($self)
--

Requests the list of active chats.

:return list of dictionaries containing data for "chatID", "title", "chatEndpointType", "chatEndpointID", "chatState".
)__");

PyDoc_STRVAR(sendMessage,
R"__(sendMessage($self, chatID, content)
--

Requests to send the given content as a message to the given chat.
The returned request ID can be used to match a call of sendMessageFinishedCallback (see setCallbacks).

:param str chatID: identifier of the chat the message should be sent to.
:param str content: the message to be sent.
:return An int identifier for the current request.
)__");

PyDoc_STRVAR(loadMessages,
R"__(loadMessages($self, chatID, messageCount, messagesBefore=None)
--

Requests to load messages from history for the given chat ID.

:param str chatID: identifier of the chat messages should be loaded from.
:param int messageCount: maximum number of messages to load.
:param str messagesBefore: load messages older than (but not including) message with given message ID.
:return True if this function call is successful, False otherwise.
)__");

PyDoc_STRVAR(deleteHistory,
R"__(deleteHistory($self, chatID)
--

Requests to delete the history for given chat.

:param str chatID: identifier of the chat to be deleted.
)__");

PyDoc_STRVAR(deleteChat,
R"__(deleteChat($self, chatID)
--

Requests to delete the chat by given chat ID.

:param str chatID: identifier of the chat to be deleted.
)__");

} // namespace DocStrings

PyMethodDef PyChatModule_methods[] =
{
	{
		"isSupported",
		WeakConnectionCall<PyChatModule, PyChatModule_isSupported>,
		METH_NOARGS,
		DocStrings::isSupported
	},
	{
		"setCallbacks",
		PyCFunctionCast(WeakConnectionCall<PyChatModule, PyChatModule_setCallbacks>),
		METH_VARARGS | METH_KEYWORDS,
		DocStrings::setCallbacks
	},
	{
		"obtainChats",
		WeakConnectionCall<PyChatModule, PyChatModule_obtainChats>,
		METH_NOARGS,
		DocStrings::obtainChats
	},
	{
		"sendMessage",
		WeakConnectionCall<PyChatModule, PyChatModule_sendMessage>,
		METH_VARARGS,
		DocStrings::sendMessage
	},
	{
		"loadMessages",
		WeakConnectionCall<PyChatModule, PyChatModule_loadMessages>,
		METH_VARARGS,
		DocStrings::loadMessages
	},
	{
		"deleteHistory",
		WeakConnectionCall<PyChatModule, PyChatModule_deleteHistory>,
		METH_O,
		DocStrings::deleteHistory
	},
	{
		"deleteChat",
		WeakConnectionCall<PyChatModule, PyChatModule_deleteChat>,
		METH_O,
		DocStrings::deleteChat
	},

	{} // Sentinel
};

} // namespace

namespace tvagentapipy
{

PyChatModule::PyChatModule(PyAgentConnection* pyAgentConnection)
	: m_pyWeakAgentConnection{reinterpret_cast<PyObject*>(pyAgentConnection)}
{
	m_module = static_cast<tvagentapi::IChatModule*>(
		pyAgentConnection->m_connection->getModule(
			tvagentapi::IModule::Type::Chat
		));
}

PyChatModule::~PyChatModule()
{
	Py_XDECREF(m_pyChatCreatedCallback);
	Py_XDECREF(m_pyChatsRemovedCallback);
	Py_XDECREF(m_pyChatClosedCallback);
	Py_XDECREF(m_pyReceivedMessagesCallback);
	Py_XDECREF(m_pySendMessageFinishedCallback);
	Py_XDECREF(m_pyMessagesLoadedCallback);
	Py_XDECREF(m_pyHistoryDeletedCallback);
}

bool PyChatModule::IsReady() const
{
	return !!m_module;
}

PyTypeObject* GetPyTypeChatModule()
{
	static PyTypeObject* pyChatModuleType = []() -> PyTypeObject*
	{
		static PyTypeObject result = PyTypeObjectInitialized();
		result.tp_name = "tvagentapi.ChatModule";
		result.tp_basicsize = sizeof(PyChatModule);
		result.tp_dealloc =
			reinterpret_cast<destructor>(DeallocWrapperObject<PyChatModule>);
		result.tp_flags = Py_TPFLAGS_DEFAULT;
		result.tp_doc = "Chat module class";
		result.tp_methods = PyChatModule_methods;

		result.tp_dict = PyDict_New();
		PyDict_SetItemString(
			result.tp_dict,
			"ChatEndpointType",
			reinterpret_cast<PyObject*>(GetPyTypeChatModule_ChatEndpointType()));
		PyDict_SetItemString(
			result.tp_dict,
			"ChatState",
			reinterpret_cast<PyObject*>(GetPyTypeChatModule_ChatState()));
		PyDict_SetItemString(
			result.tp_dict,
			"SendMessageResult",
			reinterpret_cast<PyObject*>(GetPyTypeChatModule_SendMessageResult()));

		if (PyType_Ready(&result) < 0)
		{
			return nullptr;
		}

		return &result;
	}();
	return pyChatModuleType;
}

PyTypeObject* GetPyTypeChatModule_ChatEndpointType()
{
	static PyTypeObject* pyTypeChatModule_ChatEndpointType = []() -> PyTypeObject*
	{
		using ChatEndpointType = tvagentapi::IChatModule::ChatEndpointType;
		using tvagentapi::toCString;

		PyTypeObject* result =
			CreateEnumType(
				"tvagentapi.ChatModule.ChatEndpointType",
				{{toCString(ChatEndpointType::Machine), static_cast<long>(ChatEndpointType::Machine)},
				 {toCString(ChatEndpointType::InstantSupportSession), static_cast<long>(ChatEndpointType::InstantSupportSession)}});

		return result;
	}();
	return pyTypeChatModule_ChatEndpointType;
}

PyTypeObject* GetPyTypeChatModule_ChatState()
{
	static PyTypeObject* pyTypeChatModule_ChatState = []() -> PyTypeObject*
	{
		using ChatState = tvagentapi::IChatModule::ChatState;
		using tvagentapi::toCString;

		PyTypeObject* result =
			CreateEnumType(
				"tvagentapi.ChatModule.ChatState",
				{{toCString(ChatState::Open), static_cast<long>(ChatState::Open)},
				 {toCString(ChatState::Closed), static_cast<long>(ChatState::Closed)}});

		return result;
	}();
	return pyTypeChatModule_ChatState;
}

PyTypeObject* GetPyTypeChatModule_SendMessageResult()
{
	static PyTypeObject* pyTypeChatModule_SendMessageResult = []() -> PyTypeObject*
	{
		using SendMessageResult = tvagentapi::IChatModule::SendMessageResult;
		using tvagentapi::toCString;

		PyTypeObject* result =
			CreateEnumType(
				"tvagentapi.ChatModule.SendMessageResult",
				{{toCString(SendMessageResult::Succeeded), static_cast<long>(SendMessageResult::Succeeded)},
				 {toCString(SendMessageResult::Failed), static_cast<long>(SendMessageResult::Failed)}});

		return result;
	}();
	return pyTypeChatModule_SendMessageResult;
}

} // namespace tvagentapipy
