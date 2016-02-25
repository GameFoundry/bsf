//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/** Interop class between C++ & CLR for Rigidbody. */
	class BS_SCR_BE_EXPORT ScriptRigidbody : public ScriptObject<ScriptRigidbody>
	{
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "NativeRigidbody")
	public:
		/** Returns the native Rigidbody object. */
		Rigidbody* getRigidbody() const { return mRigidbody.get(); }

	private:
		friend class ScriptColliderBase;

		ScriptRigidbody(MonoObject* instance, const SPtr<Rigidbody>& body);

		SPtr<Rigidbody> mRigidbody;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		
		// TODO - Dummy class
	};
}