#include "BsScriptObject.h"
#include "BsMonoManager.h"
#include "BsMonoField.h"

namespace BansheeEngine
{
	ScriptObjectBase::ScriptObjectBase(MonoObject* instance)
		:mManagedInstance(instance)
	{	

	}

	ScriptObjectBase::~ScriptObjectBase() 
	{
		if(mManagedInstance != nullptr)
			BS_EXCEPT(InvalidStateException, "Script object is being destroyed without its instance previously being released.");
	}

	void ScriptObjectBase::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;
		bs_delete(this);
	}
}