#include "BsScriptScriptCode.h"
#include "BsScriptResourceManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptScriptCode::ScriptScriptCode(MonoObject* instance, const HScriptCode& scriptCode)
		:ScriptObject(instance), mScriptCode(scriptCode)
	{

	}

	void ScriptScriptCode::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptScriptCode::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_GetText", &ScriptScriptCode::internal_getText);
		metaData.scriptClass->addInternalCall("Internal_SetText", &ScriptScriptCode::internal_setText);
	}

	void ScriptScriptCode::internal_createInstance(MonoObject* instance, MonoString* text)
	{
		WString strText = MonoUtil::monoToWString(text);
		HScriptCode scriptCode = ScriptCode::create(strText);

		ScriptResourceManager::instance().createScriptScriptCode(instance, scriptCode);
	}

	MonoString* ScriptScriptCode::internal_getText(ScriptScriptCode* thisPtr)
	{
		HScriptCode scriptCode = thisPtr->mScriptCode;

		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), scriptCode->getString());
	}

	void ScriptScriptCode::internal_setText(ScriptScriptCode* thisPtr, MonoString* text)
	{
		HScriptCode scriptCode = thisPtr->mScriptCode;

		scriptCode->setString(MonoUtil::monoToWString(text));
	}

	void ScriptScriptCode::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;

		if (!mRefreshInProgress)
			ScriptResourceManager::instance().destroyScriptResource(this);
	}

	void ScriptScriptCode::setNativeHandle(const HResource& resource)
	{
		mScriptCode = static_resource_cast<ScriptCode>(resource);
	}
}