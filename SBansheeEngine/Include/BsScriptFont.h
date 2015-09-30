#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsScriptObject.h"
#include "BsFont.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for Font.
	 */
	class BS_SCR_BE_EXPORT ScriptFont : public TScriptResource<ScriptFont, Font>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Font")

	private:
		friend class ScriptResourceManager;

		ScriptFont(MonoObject* instance, const HFont& font);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static MonoObject* internal_GetBitmap(ScriptFont* instance, int size);
		static int internal_GetClosestSize(ScriptFont* instance, int size);
	};

	/**
	 * @brief	Interop class between C++ & CLR for CharRange.
	 */
	class BS_SCR_BE_EXPORT ScriptCharRange : public ScriptObject <ScriptCharRange>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "CharRange")

	private:
		ScriptCharRange(MonoObject* instance);
	};
}