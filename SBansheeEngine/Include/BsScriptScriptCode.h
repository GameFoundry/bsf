#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsScriptObject.h"
#include "BsScriptCode.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for ScriptCode.
	 */
	class BS_SCR_BE_EXPORT ScriptScriptCode : public ScriptObject <ScriptScriptCode, ScriptResourceBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ScriptCode")

		/**
		 * @copydoc	ScriptResourceBase::getNativeHandle
		 */
		HResource getNativeHandle() const override { return mScriptCode; }

		/**
		 * @copydoc	ScriptResourceBase::setNativeHandle
		 */
		void setNativeHandle(const HResource& resource) override;

		/**
		 * @brief	Returns the native internal script code resource.
		 */
		HScriptCode getScriptCodeHandle() const { return mScriptCode; }
	private:
		friend class ScriptResourceManager;
		typedef std::pair<WString, WString> FullTypeName;

		ScriptScriptCode(MonoObject* instance, const HScriptCode& scriptCode);

		/**
		 * @copydoc	ScriptObjectBase::_onManagedInstanceDeleted
		 */
		void _onManagedInstanceDeleted() override;

		/**
		 * @brief	Parses the provided C# code and finds a list of all classes
		 *			and their namespaces. Nested classes are ignored.
		 */
		static Vector<FullTypeName> parseTypes(const WString& code);

		HScriptCode mScriptCode;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoString* text);
		static MonoString* internal_getText(ScriptScriptCode* thisPtr);
		static void internal_setText(ScriptScriptCode* thisPtr, MonoString* text);
		static bool internal_isEditorScript(ScriptScriptCode* thisPtr);
		static void internal_setEditorScript(ScriptScriptCode* thisPtr, bool value);
		static MonoArray* internal_getTypes(ScriptScriptCode* thisPtr);
	};
}