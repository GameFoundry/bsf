#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUISkin.
	 */
	class BS_SCR_BE_EXPORT ScriptGUISkin : public ScriptObject <ScriptGUISkin, ScriptResourceBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUISkin")

		ScriptGUISkin(MonoObject* instance, const HGUISkin& skin);

		/**
		 * @copydoc	ScriptResourceBase::getNativeHandle
		 */
		HResource getNativeHandle() const override { return mSkin; }

		/**
		 * @copydoc	ScriptResourceBase::setNativeHandle
		 */
		void setNativeHandle(const HResource& resource) override;

		/**
		 * @brief	Returns the native internal GUI skin resource.
		 */
		HGUISkin getGUIUSkinHandle() const { return mSkin; }

	private:
		/**
		 * @copydoc	ScriptObjectBase::_onManagedInstanceDeleted
		 */
		void _onManagedInstanceDeleted() override;

		HGUISkin mSkin;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_CreateInstance(MonoObject* instance);
		static MonoObject* internal_GetStyle(ScriptGUISkin* thisPtr, MonoString* name);
		static void internal_SetStyle(ScriptGUISkin* thisPtr, MonoString* name, ScriptGUIElementStyle* style);
		static void internal_RemoveStyle(ScriptGUISkin* thisPtr, MonoString* name);
		static MonoArray* internal_GetStyleNames(ScriptGUISkin* thisPtr);
	};
}