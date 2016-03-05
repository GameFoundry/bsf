//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsMonoArray.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"

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

	template<>
	String ScriptArray::get<String>(UINT32 idx)
	{
		return MonoUtil::monoToString(mono_array_get(mInternal, MonoString*, idx));
	}

	template<>
	WString ScriptArray::get<WString>(UINT32 idx)
	{
		return MonoUtil::monoToWString(mono_array_get(mInternal, MonoString*, idx));
	}

	template<>
	void ScriptArray::set<String>(UINT32 idx, const String& value)
	{
		MonoString* monoString = MonoUtil::stringToMono(value);
		mono_array_set(mInternal, MonoString*, idx, monoString);
	}

	template<>
	void ScriptArray::set<WString>(UINT32 idx, const WString& value)
	{
		MonoString* monoString = MonoUtil::wstringToMono(value);
		mono_array_set(mInternal, MonoString*, idx, monoString);
	}

	UINT32 ScriptArray::size() const
	{
		return (UINT32)mono_array_length(mInternal);
	}

	UINT32 ScriptArray::elementSize() const
	{
		::MonoClass* arrayClass = mono_object_get_class((MonoObject*)(mInternal));
		::MonoClass* elementClass = mono_class_get_element_class(arrayClass);

		return (UINT32)mono_class_array_element_size(elementClass);
	}

	template BS_MONO_EXPORT String ScriptArray::get(UINT32 idx);
	template BS_MONO_EXPORT WString ScriptArray::get(UINT32 idx);

	template BS_MONO_EXPORT void ScriptArray::set(UINT32 idx, const String& value);
	template BS_MONO_EXPORT void ScriptArray::set(UINT32 idx, const WString& value);
}