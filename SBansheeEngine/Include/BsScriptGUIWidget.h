#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**	Interop class between C++ & CLR for GUIWidget. */
	class BS_SCR_BE_EXPORT ScriptGUIWidget : public ScriptObject <ScriptGUIWidget>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "NativeGUIWidget")

		/**	Gets the wrapped native GUIWidget object. */
		SPtr<GUIWidget> getInternal() const { return mGUIWidget; }

	private:
		ScriptGUIWidget(MonoObject* managedInstance);
		~ScriptGUIWidget();

		/**	Destroys the internal GUI widget handler object. */
		void destroy();

		/** @copydoc ScriptObject::_onManagedInstanceDeleted */
		void _onManagedInstanceDeleted() override;

		SPtr<GUIWidget> mGUIWidget;
		ScriptGUILayout* mPanel;
		UINT32 mLastUpdateHash;

		static MonoField* sGUIPanelField;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_Create(MonoObject* instance);
		static void internal_UpdateTransform(ScriptGUIWidget* instance, ScriptSceneObject* parentSO);
		static void internal_UpdateMainCamera(ScriptGUIWidget* instance, ScriptCamera* camera);
		static void internal_SetSkin(ScriptGUIWidget* instance, ScriptGUISkin* skin);
		static void internal_SetCamera(ScriptGUIWidget* instance, ScriptCamera* camera);
		static void internal_Destroy(ScriptGUIWidget* instance);
	};
}