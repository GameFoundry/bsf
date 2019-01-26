#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../../../Foundation/bsfCore/Image/BsSpriteTexture.h"

namespace bs
{
	class BS_SCR_BE_EXPORT ScriptSpriteSheetGridAnimation : public ScriptObject<ScriptSpriteSheetGridAnimation>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "bs", "SpriteSheetGridAnimation")

		static MonoObject* box(const SpriteSheetGridAnimation& value);
		static SpriteSheetGridAnimation unbox(MonoObject* value);

	private:
		ScriptSpriteSheetGridAnimation(MonoObject* managedInstance);

	};
}
