#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSpriteTexture : public ScriptResource, public ScriptObject<ScriptSpriteTexture>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "SpriteTexture")

		const HSpriteTexture& getInternalValue() const { return mTexture; }

		HResource getNativeHandle() const { return mTexture; }
		void setNativeHandle(const HResource& resource);

	private:
		friend class ScriptResourceManager;

		static void internal_createInstance(MonoObject* instance);

		ScriptSpriteTexture(MonoObject* instance, const HSpriteTexture& texture);

		void _onManagedInstanceDeleted();

		HSpriteTexture mTexture;
	};
}