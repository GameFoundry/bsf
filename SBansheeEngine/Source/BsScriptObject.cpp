#include "BsScriptObject.h"
#include "BsMonoManager.h"
#include "BsMonoField.h"

namespace BansheeEngine
{
	ScriptMeta ScriptObjectBase::metaData;

	ScriptObjectBase::ScriptObjectBase(MonoObject* instance)
		:mManagedInstance(instance)
	{	
		ScriptObjectBase* param = this;

		if(metaData.thisPtrField != nullptr)
			metaData.thisPtrField->setValue(instance, &param);
	}

	ScriptObjectBase::~ScriptObjectBase() 
	{
		if(mManagedInstance != nullptr)
			CM_EXCEPT(InvalidStateException, "Script object is being destroyed without its instance previously being released.");
	}

	void ScriptObjectBase::_onManagedInstanceDeleted()
	{
		cm_delete(this);
	}

	void ScriptObjectBase::registerTypeWithManager()
	{
		MonoManager::registerScriptType(&metaData);
	}

	void* ScriptObjectBase::getNativeInstance(MonoObject* managedInstance)
	{
		void* nativeInstance = nullptr;

		if(metaData.thisPtrField != nullptr)
			metaData.thisPtrField->getValue(managedInstance, &nativeInstance);

		return nativeInstance;
	}
}