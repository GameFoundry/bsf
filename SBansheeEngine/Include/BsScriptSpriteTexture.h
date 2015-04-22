#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSpriteTexture : public ScriptObject<ScriptSpriteTexture, ScriptResourceBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "SpriteTexture")

		const HSpriteTexture& getInternalValue() const { return mTexture; }

		HResource getNativeHandle() const { return mTexture; }
		void setNativeHandle(const HResource& resource);

		static MonoObject* toManaged(const HSpriteTexture& texture);

	private:
		friend class ScriptResourceManager;

		static void internal_createInstance(MonoObject* instance, MonoObject* texture, Vector2 offset, Vector2 scale);

		ScriptSpriteTexture(MonoObject* instance, const HSpriteTexture& texture);

		void _onManagedInstanceDeleted();

		HSpriteTexture mTexture;
	};
}