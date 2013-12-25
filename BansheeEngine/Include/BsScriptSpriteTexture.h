#pragma once

#include "BsPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_EXPORT ScriptSpriteTexture : public ScriptObject<ScriptSpriteTexture>
	{
	public:
		static void initMetaData();

		const SpriteTexturePtr& getInternalValue() const { return mTexture; }

	private:
		static void internal_createInstance(MonoObject* instance);
		static void internal_destroyInstance(ScriptSpriteTexture* nativeInstance);

		static void initRuntimeData();

		ScriptSpriteTexture(const SpriteTexturePtr& texture);

		SpriteTexturePtr mTexture;
	};
}