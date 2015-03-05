#include "BsScriptFont.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsSpriteTexture.h"
#include "BsScriptResourceManager.h"

namespace BansheeEngine
{
	ScriptFont::ScriptFont(MonoObject* instance, const HFont& font)
		:ScriptObject(instance), mFont(font)
	{

	}

	void ScriptFont::initRuntimeData()
	{

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

	ScriptCharRange::ScriptCharRange(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptCharRange::initRuntimeData()
	{ }
}