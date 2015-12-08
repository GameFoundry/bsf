#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for ResourceRef.
	 */
	class BS_SCR_BE_EXPORT ScriptResourceRef : public ScriptObject<ScriptResourceRef>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ResourceRef`1")

		/**
		 * @brief	Creates a new managed ResourceRef for the provided resource type.
		 *
		 * @param	typeId	RTTI type ID of the resource to reference.
		 */
		static MonoObject* create(UINT32 typeId);

	private:
		ScriptResourceRef(MonoObject* instance, const WeakResourceHandle<Resource>& resource);

		WeakResourceHandle<Resource> mResource;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static bool internal_IsLoaded(ScriptResourceRef* nativeInstance);
		static MonoObject* internal_GetResource(ScriptResourceRef* nativeInstance);
	};
}