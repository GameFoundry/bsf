#include "BsScriptPlatformInfo.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"

namespace BansheeEngine
{
	ScriptPlatformInfoBase::ScriptPlatformInfoBase(MonoObject* instance)
		:ScriptObjectBase(instance)
	{ }

	ScriptPlatformInfo::ScriptPlatformInfo(MonoObject* instance)
		: ScriptObject(instance)
	{

	}

	void ScriptPlatformInfo::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("internal_GetType", &ScriptPlatformInfo::internal_GetType);
		metaData.scriptClass->addInternalCall("internal_GetDefines", &ScriptPlatformInfo::internal_GetDefines);
		metaData.scriptClass->addInternalCall("internal_SetDefines", &ScriptPlatformInfo::internal_SetDefines);
	}

	MonoObject* ScriptPlatformInfo::create(const SPtr<PlatformInfo>& platformInfo)
	{
		switch (platformInfo->type)
		{
		case PlatformType::Windows:
			return ScriptWinPlatformInfo::create(std::static_pointer_cast<WinPlatformInfo>(platformInfo));
			break;
		}

		return nullptr;
	}

	PlatformType ScriptPlatformInfo::internal_GetType(ScriptPlatformInfoBase* thisPtr)
	{
		return thisPtr->getPlatformInfo()->type;
	}

	MonoString* ScriptPlatformInfo::internal_GetDefines(ScriptPlatformInfoBase* thisPtr)
	{
		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), thisPtr->getPlatformInfo()->defines);
	}

	void ScriptPlatformInfo::internal_SetDefines(ScriptPlatformInfoBase* thisPtr, MonoString* value)
	{
		thisPtr->getPlatformInfo()->defines = MonoUtil::monoToWString(value);
	}

	ScriptWinPlatformInfo::ScriptWinPlatformInfo(MonoObject* instance)
		:ScriptObject(instance)
	{
		mPlatformInfo = bs_shared_ptr<WinPlatformInfo>();
	}

	void ScriptWinPlatformInfo::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("internal_GetIs32Bit", &ScriptWinPlatformInfo::internal_GetIs32Bit);
		metaData.scriptClass->addInternalCall("internal_SetIs32Bit", &ScriptWinPlatformInfo::internal_SetIs32Bit);
	}

	SPtr<WinPlatformInfo> ScriptWinPlatformInfo::getWinPlatformInfo() const
	{
		return std::static_pointer_cast<WinPlatformInfo>(mPlatformInfo);
	}

	MonoObject* ScriptWinPlatformInfo::create(const SPtr<WinPlatformInfo>& platformInfo)
	{
		MonoObject* managedInstance = metaData.scriptClass->createInstance();
		ScriptWinPlatformInfo* scriptObj = ScriptWinPlatformInfo::toNative(managedInstance);
		scriptObj->mPlatformInfo = platformInfo;

		return managedInstance;
	}

	bool ScriptWinPlatformInfo::internal_GetIs32Bit(ScriptWinPlatformInfo* thisPtr)
	{
		return thisPtr->getWinPlatformInfo()->is32bit;
	}

	void ScriptWinPlatformInfo::internal_SetIs32Bit(ScriptWinPlatformInfo* thisPtr, bool value)
	{
		thisPtr->getWinPlatformInfo()->is32bit = value;
	}
}