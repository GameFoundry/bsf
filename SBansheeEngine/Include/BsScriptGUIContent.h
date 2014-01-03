#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIContent : public ScriptObject<ScriptGUIContent>
	{
	public:
		static void initMetaData();

		static const CM::HString& getText(MonoObject* instance);
		static const CM::HString& getTooltip(MonoObject* instance);
		static HSpriteTexture getImage(MonoObject* instance);

	private:
		static void initRuntimeData();

		static MonoField* mTextField;
		static MonoField* mTooltipField;
		static MonoField* mImageField;
	};
}