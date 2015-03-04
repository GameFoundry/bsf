#include "BsMonoArray.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"

namespace BansheeEngine
{
	ScriptArray::ScriptArray(MonoArray* existingArray)
		:mInternal(existingArray)
	{

	}

	ScriptArray::ScriptArray(MonoClass& klass, UINT32 size)
		: mInternal(nullptr)
	{
		mInternal = mono_array_new(MonoManager::instance().getDomain(), klass._getInternalClass(), size);
	}

	ScriptArray::ScriptArray(::MonoClass* klass, UINT32 size)
		: mInternal(nullptr)
	{
		mInternal = mono_array_new(MonoManager::instance().getDomain(), klass, size);
	}

	UINT32 ScriptArray::size() const
	{
		return (UINT32)mono_array_length(mInternal);
	}
}