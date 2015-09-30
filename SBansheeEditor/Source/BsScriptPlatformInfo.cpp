#include "BsScriptPlatformInfo.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsScriptTexture2D.h"
#include "BsScriptResourceManager.h"

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
		metaData.scriptClass->addInternalCall("Internal_GetType", &ScriptPlatformInfo::internal_GetType);
		metaData.scriptClass->addInternalCall("Internal_GetDefines", &ScriptPlatformInfo::internal_GetDefines);
		metaData.scriptClass->addInternalCall("Internal_SetDefines", &ScriptPlatformInfo::internal_SetDefines);
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
		mPlatformInfo = bs_shared_ptr_new<WinPlatformInfo>();
	}

	void ScriptWinPlatformInfo::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetIs32Bit", &ScriptWinPlatformInfo::internal_GetIs32Bit);
		metaData.scriptClass->addInternalCall("Internal_SetIs32Bit", &ScriptWinPlatformInfo::internal_SetIs32Bit);
		metaData.scriptClass->addInternalCall("Internal_GetIcon", &ScriptWinPlatformInfo::internal_GetIcon);
		metaData.scriptClass->addInternalCall("Internal_SetIcon", &ScriptWinPlatformInfo::Internal_SetIcon);
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

	MonoObject* ScriptWinPlatformInfo::internal_GetIcon(ScriptWinPlatformInfo* thisPtr, int size)
	{
		HTexture icon;
		switch (size)
		{
		case 16:
			icon = thisPtr->getWinPlatformInfo()->icon16;
			break;
		case 32:
			icon = thisPtr->getWinPlatformInfo()->icon32;
			break;
		case 48:
			icon = thisPtr->getWinPlatformInfo()->icon48;
			break;
		case 64:
			icon = thisPtr->getWinPlatformInfo()->icon64;
			break;
		case 96:
			icon = thisPtr->getWinPlatformInfo()->icon96;
			break;
		case 128:
			icon = thisPtr->getWinPlatformInfo()->icon128;
			break;
		case 192:
			icon = thisPtr->getWinPlatformInfo()->icon192;
			break;
		case 256:
			icon = thisPtr->getWinPlatformInfo()->icon256;
			break;
		}

		if (icon != nullptr)
		{
			ScriptTexture2D* scriptTexture;
			ScriptResourceManager::instance().getScriptResource(icon, &scriptTexture, true);

			return scriptTexture->getManagedInstance();
		}

		return nullptr;
	}

	void ScriptWinPlatformInfo::Internal_SetIcon(ScriptWinPlatformInfo* thisPtr, int size, ScriptTexture2D* texturePtr)
	{
		HTexture icon;

		if (texturePtr != nullptr)
			icon = texturePtr->getHandle();

		switch (size)
		{
		case 16:
			thisPtr->getWinPlatformInfo()->icon16 = icon;
			break;
		case 32:
			thisPtr->getWinPlatformInfo()->icon32 = icon;
			break;
		case 48:
			thisPtr->getWinPlatformInfo()->icon48 = icon;
			break;
		case 64:
			thisPtr->getWinPlatformInfo()->icon64 = icon;
			break;
		case 96:
			thisPtr->getWinPlatformInfo()->icon96 = icon;
			break;
		case 128:
			thisPtr->getWinPlatformInfo()->icon128 = icon;
			break;
		case 192:
			thisPtr->getWinPlatformInfo()->icon192 = icon;
			break;
		case 256:
			thisPtr->getWinPlatformInfo()->icon256 = icon;
			break;
		}
	}
}