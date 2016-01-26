#include "BsScriptObjectImpl.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"

namespace BansheeEngine
{
	ScriptObjectImpl::ScriptObjectImpl(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptObjectImpl::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_ManagedInstanceDeleted", &ScriptObjectImpl::internal_managedInstanceDeleted);
	}

	void ScriptObjectImpl::internal_managedInstanceDeleted(ScriptObjectBase* instance)
	{
		instance->_onManagedInstanceDeleted();
	}
}