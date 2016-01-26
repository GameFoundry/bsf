#include "BsScriptFont.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsSpriteTexture.h"

namespace BansheeEngine
{
	ScriptFont::ScriptFont(MonoObject* instance, const HFont& font)
		:ScriptObject(instance), mFont(font)
	{

	}

	void* ScriptFont::getNativeRaw() const
	{
		return (void*)mFont.get();
	}

	void ScriptFont::initRuntimeData()
	{

	}

	void ScriptFont::internal_createInstanceExternal(MonoObject* instance, const HFont& font)
	{
		ScriptFont* nativeInstance = new (bs_alloc<ScriptFont>()) ScriptFont(instance, font);
	}
}