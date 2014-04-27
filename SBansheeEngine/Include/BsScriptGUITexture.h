#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsGUITexture.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUITexture : public ScriptObject<ScriptGUITexture>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUITexture")

		GUITexture* getInternalValue() const { return mTexture; }
		void* getNativeRaw() const { return mTexture; }

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* texture, 
			GUIImageScaleMode scale, MonoString* style, MonoArray* guiOptions);
		static void internal_setTexture(ScriptGUITexture* nativeInstance, MonoObject* texture);

		static void internal_destroy(ScriptGUITexture* nativeInstance);
		static void internal_setVisible(ScriptGUITexture* nativeInstance, bool visible);
		static void internal_setParent(ScriptGUITexture* nativeInstance, MonoObject* parentLayout);

		ScriptGUITexture(MonoObject* instance, GUITexture* texture);

		void destroy();
		void _onManagedInstanceDeleted();

		GUITexture* mTexture;
		bool mIsDestroyed;
	};
}