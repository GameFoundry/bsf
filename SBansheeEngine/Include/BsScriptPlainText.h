#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsScriptObject.h"
#include "BsPlainText.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for PlainText.
	 */
	class BS_SCR_BE_EXPORT ScriptPlainText : public ScriptObject <ScriptPlainText, ScriptResourceBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "PlainText")

		/**
		 * @copydoc	ScriptResourceBase::getNativeHandle
		 */
		HResource getNativeHandle() const override { return mPlainText; }

		/**
		 * @copydoc	ScriptResourceBase::setNativeHandle
		 */
		void setNativeHandle(const HResource& resource) override;

		/**
		 * @brief	Returns the internal wrapped plain text resource.
		 */
		HPlainText getPlainTextHandle() const { return mPlainText; }
	private:
		friend class ScriptResourceManager;

		ScriptPlainText(MonoObject* instance, const HPlainText& plainText);

		/**
		 * @copydoc	ScriptObjectBase::_onManagedInstanceDeleted
		 */
		void _onManagedInstanceDeleted() override;

		HPlainText mPlainText;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoString* text);
		static MonoString* internal_getText(ScriptPlainText* thisPtr);
		static void internal_setText(ScriptPlainText* thisPtr, MonoString* text);
	};
}