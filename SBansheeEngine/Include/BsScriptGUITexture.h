#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsGUITexture.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUITexture : public ScriptObject<ScriptGUITexture>
	{
	public:
		static void initMetaData();

		GUITexture* getInternalValue() const { return mTexture; }
		void* getNativeRaw() const { return mTexture; }

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* parentLayout, MonoObject* texture, 
			GUIImageScaleMode scale, MonoObject* style, MonoArray* guiOptions);
		static void internal_destroyInstance(ScriptGUITexture* nativeInstance);
		static void internal_setTexture(ScriptGUITexture* nativeInstance, MonoObject* texture);

		static void initRuntimeData();

		ScriptGUITexture(GUITexture* texture);

		GUITexture* mTexture;
	};
}