#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIContent : public ScriptObject<ScriptGUIContent>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUIContent")

		static const HString& getText(MonoObject* instance);
		static const HString& getTooltip(MonoObject* instance);
		static HSpriteTexture getImage(MonoObject* instance);

	private:
		static MonoField* mTextField;
		static MonoField* mTooltipField;
		static MonoField* mImageField;

		ScriptGUIContent(MonoObject* instance);
	};
}