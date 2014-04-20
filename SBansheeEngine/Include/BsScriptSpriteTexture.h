#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSpriteTexture : public ScriptResource, public ScriptObject<ScriptSpriteTexture>
	{
	public:
		static void initMetaData();

		const HSpriteTexture& getInternalValue() const { return mTexture; }

		HResource getNativeHandle() const { return mTexture; }
		void setNativeHandle(const HResource& resource);

	private:
		friend class ScriptResourceManager;

		static void internal_createInstance(MonoObject* instance);
		static void internal_destroyInstance(ScriptSpriteTexture* nativeInstance);

		static void initRuntimeData();

		ScriptSpriteTexture(const HSpriteTexture& texture);
		~ScriptSpriteTexture() {}

		HSpriteTexture mTexture;
	};
}