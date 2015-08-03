#include "BsScriptPlainText.h"
#include "BsScriptResourceManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsPlainText.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptPlainText::ScriptPlainText(MonoObject* instance, const HPlainText& plainText)
		:ScriptObject(instance), mPlainText(plainText)
	{

	}

	void ScriptPlainText::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptPlainText::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_GetText", &ScriptPlainText::internal_getText);
		metaData.scriptClass->addInternalCall("Internal_SetText", &ScriptPlainText::internal_setText);
	}

	void ScriptPlainText::internal_createInstance(MonoObject* instance, MonoString* text)
	{
		WString strText = MonoUtil::monoToWString(text);
		HPlainText plainText = PlainText::create(strText);

		ScriptPlainText* scriptInstance;
		ScriptResourceManager::instance().createScriptResource(instance, plainText, &scriptInstance);
	}

	MonoString* ScriptPlainText::internal_getText(ScriptPlainText* thisPtr)
	{
		HPlainText plainText = thisPtr->mPlainText;

		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), plainText->getString());
	}

	void ScriptPlainText::internal_setText(ScriptPlainText* thisPtr, MonoString* text)
	{
		HPlainText plainText = thisPtr->mPlainText;

		plainText->setString(MonoUtil::monoToWString(text));
	}

	void ScriptPlainText::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;

		if (!mRefreshInProgress)
			ScriptResourceManager::instance().destroyScriptResource(this);
	}

	void ScriptPlainText::setNativeHandle(const HResource& resource)
	{
		mPlainText = static_resource_cast<PlainText>(resource);
	}
}