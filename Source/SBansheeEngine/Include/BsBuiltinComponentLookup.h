//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptMeta.h"
#include "BsMonoClass.h"

#include "BsScriptRenderable.h"

namespace bs
{
	/** Begins the definition for the builtin component lookup table. */
#define LOOKUP_BEGIN																										\
	class BuiltinComponents																									\
	{																														\
	public:																													\
		template<int TID>																									\
		class TLookup { };																									\
																															\
	private:																												\
		struct META_FirstEntry {};																							\
		static void META_GetPrevEntries(Vector<BuiltinComponentInfo>& entries, META_FirstEntry id) { }						\
																															\
		typedef META_FirstEntry 

	/** Registers a new entry in the component lookup table. */
#define ADD_ENTRY(TID, Type)																								\
		META_Entry_##Type;																									\
																															\
	public:																													\
		template<>																											\
		class TLookup<TID>																									\
		{																													\
			typedef Type ScriptType;																						\
		};																													\
																															\
		static ScriptComponentBase* create##Type(const HComponent& component)												\
		{																													\
			MonoObject* managedInstance = Type::getMetaData()->scriptClass->createInstance();								\
			Type* scriptComponent = new (bs_alloc<Type>()) Type(managedInstance, component);								\
																															\
			return scriptComponent;																							\
		}																													\
																															\
		struct META_NextEntry_##Type {};																					\
		static void META_GetPrevEntries(Vector<BuiltinComponentInfo>& entries, META_NextEntry_##Type id)					\
		{																													\
			META_GetPrevEntries(entries, META_Entry_##Type());																\
																															\
			BuiltinComponentInfo entry;																						\
			entry.metaData = Type::getMetaData();																			\
			entry.typeId = TID;																								\
			entry.monoClass = nullptr;																						\
			entry.createCallback = &create##Type;																			\
																															\
			entries.push_back(entry);																						\
		}																													\
																															\
		typedef META_NextEntry_##Type

	/** End the definition for the builtin component lookup table. */
#define LOOKUP_END																											\
		META_LastEntry;																										\
																															\
	public:																													\
		static Vector<BuiltinComponentInfo> getEntries()																	\
		{																													\
			Vector<BuiltinComponentInfo> entries;																			\
			META_GetPrevEntries(entries, META_LastEntry());																	\
			return entries;																									\
		}																													\
	};

	LOOKUP_BEGIN
		ADD_ENTRY(TID_Renderable, ScriptRenderable2)
	LOOKUP_END

#undef LOOKUP_BEGIN
#undef ADD_ENTRY
#undef LOOKUP_END
}