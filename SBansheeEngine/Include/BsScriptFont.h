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
	class BS_SCR_BE_EXPORT ScriptFont : public ScriptObject<ScriptFont, ScriptResourceBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Font")

		/**
		 * @copydoc	ScriptResourceBase::getNativeHandle
		 */
		HResource getNativeHandle() const override { return mFont; }

		/**
		 * @copydoc	ScriptResourceBase::setNativeHandle
		 */
		void setNativeHandle(const HResource& resource) override;
	private:
		friend class ScriptResourceManager;

		ScriptFont(MonoObject* instance, const HFont& font);

		/**
		 * @copydoc	ScriptObjectBase::_onManagedInstanceDeleted
		 */
		void _onManagedInstanceDeleted() override;

		HFont mFont;

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