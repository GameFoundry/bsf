#include "BsScriptFont.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptResourceManager.h"
#include "BsScriptFontBitmap.h"

namespace BansheeEngine
{
	ScriptFont::ScriptFont(MonoObject* instance, const HFont& font)
		:ScriptObject(instance), mFont(font)
	{

	}

	void ScriptFont::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetBitmap", &ScriptFont::internal_GetBitmap);
		metaData.scriptClass->addInternalCall("Internal_GetClosestSize", &ScriptFont::internal_GetClosestSize);
	}

	void ScriptFont::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;

		if (!mRefreshInProgress)
			ScriptResourceManager::instance().destroyScriptResource(this);
	}

	void ScriptFont::setNativeHandle(const HResource& resource)
	{
		mFont = static_resource_cast<Font>(resource);
	}

	MonoObject* ScriptFont::internal_GetBitmap(ScriptFont* instance, int size)
	{
		if (!instance->mFont.isLoaded())
			return nullptr;

		SPtr<const FontBitmap> bitmap = instance->mFont->getBitmap(size);
		if (bitmap != nullptr)
			return ScriptFontBitmap::create(bitmap);

		return nullptr;
	}

	int ScriptFont::internal_GetClosestSize(ScriptFont* instance, int size)
	{
		if (!instance->mFont.isLoaded())
			return 0;

		return instance->mFont->getClosestSize(size);
	}

	ScriptCharRange::ScriptCharRange(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptCharRange::initRuntimeData()
	{ }
}