//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsAnimation.h"

namespace BansheeEngine
{
	class ScriptAnimationClip;

	/** @addtogroup ScriptInteropEngine
	 *  @{
	 */

	/**	Interop class between C++ & CLR for MorphShapes. */
	class BS_SCR_BE_EXPORT ScriptMorphShapes : public ScriptObject <ScriptMorphShapes>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "MorphShapes")

		/**	Returns the native wrapped object. */
		SPtr<MorphShapes> getInternal() const { return mMorphShapes; }

		/** Creates a managed object from the native morph shapes object. */
		static MonoObject* create(const SPtr<MorphShapes>& morphShapes);

	private:
		ScriptMorphShapes(MonoObject* managedInstance, const SPtr<MorphShapes>& morphShapes);

		SPtr<MorphShapes> mMorphShapes;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static MonoArray* internal_GetShapes(ScriptMorphShapes* thisPtr);
	};

	/** @} */
}