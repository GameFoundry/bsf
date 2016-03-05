//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsTexture.h"
#include "BsScriptResource.h"
#include "BsMonoClass.h"
#include "BsRTTIType.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for ResourceRef.
	 */
	class BS_SCR_BE_EXPORT ScriptResourceRef : public ScriptObject<ScriptResourceRef>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ResourceRef")

		/**
		 * @brief	Creates a new managed ResourceRef for the provided resource.
		 *
		 * @param	handle	Handle to the resource to wrap.
		 */
		template<class T>
		static MonoObject* create(const WeakResourceHandle<T>& handle)
		{
			return createInternal(handle);
		}

		/**
		 * @brief	Creates a new managed ResourceRef for the provided texture.
		 *
		 * @param	handle	Handle to the texture to wrap.
		 */
		static MonoObject* create(const WeakResourceHandle<Texture>& handle, TextureType type = TEX_TYPE_2D);

		/**
		 * @brief	Returns a weak handle to the resource referenced by this object.
		 */
		WeakResourceHandle<Resource> getHandle() const { return mResource; }

	private:
		friend class ScriptResourceRefBase;

		ScriptResourceRef(MonoObject* instance, const WeakResourceHandle<Resource>& handle);

		/**
		 * @brief	Creates a new managed ResourceRef for the provided resource type.
		 *
		 * @param	handle			Handle to the resource to wrap.
		 */
		static MonoObject* createInternal(const WeakResourceHandle<Resource>& handle);

		WeakResourceHandle<Resource> mResource;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static bool internal_IsLoaded(ScriptResourceRef* nativeInstance);
		static MonoObject* internal_GetResource(ScriptResourceRef* nativeInstance);
		static MonoString* internal_GetUUID(ScriptResourceRef* thisPtr);
	};
}