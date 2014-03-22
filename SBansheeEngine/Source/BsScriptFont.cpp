#include "BsScriptFont.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
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
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "Font", &ScriptFont::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void* ScriptFont::getNativeRaw() const
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

		metaData.thisPtrField->setValue(instance, &nativeInstance);
	}

	void ScriptFont::internal_destroyInstance(ScriptFont* nativeInstance)
	{
		cm_delete(nativeInstance);
	}
}