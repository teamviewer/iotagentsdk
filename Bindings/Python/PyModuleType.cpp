#include "PyModuleType.h"

#include <TVAgentAPI/IModule.h>
#include <TVAgentAPI/ModuleStringify.h>

#include "PythonHelpers.h"

namespace tvagentapipy
{

PyTypeObject* GetPyTypeModule_Type()
{
	static PyTypeObject* pyTypeModule_Type = []() -> PyTypeObject*
	{
		using Type = tvagentapi::IModule::Type;
		using tvagentapi::toCString;

		PyTypeObject* result =
			CreateEnumType(
				"tvagentapi.ModuleType",
				{{toCString(Type::AccessControl),       static_cast<long>(Type::AccessControl)},
				 {toCString(Type::InstantSupport),      static_cast<long>(Type::InstantSupport)},
				 {toCString(Type::TVSessionManagement), static_cast<long>(Type::TVSessionManagement)},
				 {toCString(Type::Chat),                static_cast<long>(Type::Chat)},
				 {toCString(Type::AugmentRCSession),    static_cast<long>(Type::AugmentRCSession)}});
		return result;
	}();
	return pyTypeModule_Type;
}

} // namespace tvagentapipy
