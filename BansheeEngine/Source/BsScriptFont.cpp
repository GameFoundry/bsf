#include "BsScriptFont.h"
#include "BsScriptMeta.h"
#include "BsScriptField.h"
#include "BsScriptClass.h"
#include "BsScriptManager.h"
#include "BsSpriteTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptFont::ScriptFont(const CM::HFont& font)
		:mFont(font)
	{

	}

	void ScriptFont::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "Font", &ScriptFont::initRuntimeData);

		ScriptManager::registerScriptType(&metaData);
	}

	void* ScriptFont::getNative() const
	{
		return (void*)mFont.get();
	}

	void ScriptFont::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptFont::internal_destroyInstance);
	}

	void ScriptFont::internal_createInstanceExternal(MonoObject* instance, const CM::HFont& font)
	{
		ScriptFont* nativeInstance = new (cm_alloc<ScriptFont>()) ScriptFont(font);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptFont::internal_destroyInstance(ScriptFont* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}
}