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
			CM_EXCEPT(InvalidStateException, "Script object is being destroyed without its instance previously being released.");
	}

	static Vector<UINT64> dbgList;
	void ScriptObjectBase::_onManagedInstanceDeleted()
	{
		auto iterFind = std::find(dbgList.begin(), dbgList.end(), (UINT64)this);
		if (iterFind != dbgList.end())
		{
			assert(false);
		}

		dbgList.push_back((UINT64)this);

		mManagedInstance = nullptr;
		cm_delete(this);
	}
}