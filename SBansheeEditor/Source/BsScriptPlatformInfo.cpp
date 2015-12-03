#include "BsScriptPlatformInfo.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsScriptTexture2D.h"
#include "BsScriptResourceManager.h"
#include "BsScriptPrefab.h"

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
		metaData.scriptClass->addInternalCall("Internal_GetMainScene", &ScriptPlatformInfo::internal_GetMainScene);
		metaData.scriptClass->addInternalCall("Internal_SetMainScene", &ScriptPlatformInfo::internal_SetMainScene);
		metaData.scriptClass->addInternalCall("Internal_GetFullscreen", &ScriptPlatformInfo::internal_GetFullscreen);
		metaData.scriptClass->addInternalCall("Internal_SetFullscreen", &ScriptPlatformInfo::internal_SetFullscreen);
		metaData.scriptClass->addInternalCall("Internal_GetResolution", &ScriptPlatformInfo::internal_GetResolution);
		metaData.scriptClass->addInternalCall("Internal_SetResolution", &ScriptPlatformInfo::internal_SetResolution);
		metaData.scriptClass->addInternalCall("Internal_GetDebug", &ScriptPlatformInfo::internal_GetDebug);
		metaData.scriptClass->addInternalCall("Internal_SetDebug", &ScriptPlatformInfo::internal_SetDebug);
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

	MonoObject* ScriptPlatformInfo::internal_GetMainScene(ScriptPlatformInfoBase* thisPtr)
	{
		HPrefab prefab = thisPtr->getPlatformInfo()->mainScene;

		if (prefab != nullptr)
		{
			ScriptPrefab* scriptPrefab;
			ScriptResourceManager::instance().getScriptResource(prefab, &scriptPrefab, true);

			return scriptPrefab->getManagedInstance();
		}

		return nullptr;
	}

	void ScriptPlatformInfo::internal_SetMainScene(ScriptPlatformInfoBase* thisPtr, ScriptPrefab* prefabPtr)
	{
		HPrefab prefab;

		if (prefabPtr != nullptr)
			prefab = prefabPtr->getHandle();

		thisPtr->getPlatformInfo()->mainScene = prefab;
	}

	bool ScriptPlatformInfo::internal_GetFullscreen(ScriptPlatformInfoBase* thisPtr)
	{
		return thisPtr->getPlatformInfo()->fullscreen;
	}

	void ScriptPlatformInfo::internal_SetFullscreen(ScriptPlatformInfoBase* thisPtr, bool fullscreen)
	{
		thisPtr->getPlatformInfo()->fullscreen = fullscreen;
	}

	void ScriptPlatformInfo::internal_GetResolution(ScriptPlatformInfoBase* thisPtr, UINT32* width, UINT32* height)
	{
		*width = thisPtr->getPlatformInfo()->windowedWidth;
		*height = thisPtr->getPlatformInfo()->windowedHeight;
	}

	void ScriptPlatformInfo::internal_SetResolution(ScriptPlatformInfoBase* thisPtr, UINT32 width, UINT32 height)
	{
		thisPtr->getPlatformInfo()->windowedWidth = width;
		thisPtr->getPlatformInfo()->windowedHeight = height;
	}

	bool ScriptPlatformInfo::internal_GetDebug(ScriptPlatformInfoBase* thisPtr)
	{
		return thisPtr->getPlatformInfo()->debug;
	}

	void ScriptPlatformInfo::internal_SetDebug(ScriptPlatformInfoBase* thisPtr, bool debug)
	{
		thisPtr->getPlatformInfo()->debug = debug;
	}

	ScriptWinPlatformInfo::ScriptWinPlatformInfo(MonoObject* instance)
		:ScriptObject(instance)
	{
		mPlatformInfo = bs_shared_ptr_new<WinPlatformInfo>();
	}

	void ScriptWinPlatformInfo::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetIcon", &ScriptWinPlatformInfo::internal_GetIcon);
		metaData.scriptClass->addInternalCall("Internal_SetIcon", &ScriptWinPlatformInfo::internal_SetIcon);
		metaData.scriptClass->addInternalCall("Internal_GetTaskbarIcon", &ScriptWinPlatformInfo::internal_GetTaskbarIcon);
		metaData.scriptClass->addInternalCall("Internal_SetTaskbarIcon", &ScriptWinPlatformInfo::internal_SetTaskbarIcon);
		metaData.scriptClass->addInternalCall("Internal_GetTitleText", &ScriptWinPlatformInfo::internal_GetTitleText);
		metaData.scriptClass->addInternalCall("Internal_SetTitleText", &ScriptWinPlatformInfo::internal_SetTitleText);
	}

	SPtr<WinPlatformInfo> ScriptWinPlatformInfo::getWinPlatformInfo() const
	{
		return std::static_pointer_cast<WinPlatformInfo>(mPlatformInfo);
	}

	MonoObject* ScriptWinPlatformInfo::create(const SPtr<WinPlatformInfo>& platformInfo)
	{
		MonoObject* managedInstance = metaData.scriptClass->createInstance();
		ScriptWinPlatformInfo* scriptObj = new (bs_alloc<ScriptWinPlatformInfo>()) ScriptWinPlatformInfo(managedInstance);
		scriptObj->mPlatformInfo = platformInfo;

		return managedInstance;
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

	void ScriptWinPlatformInfo::internal_SetIcon(ScriptWinPlatformInfo* thisPtr, int size, ScriptTexture2D* texturePtr)
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

	MonoObject* ScriptWinPlatformInfo::internal_GetTaskbarIcon(ScriptWinPlatformInfo* thisPtr)
	{
		HTexture icon = thisPtr->getWinPlatformInfo()->taskbarIcon;

		if (icon != nullptr)
		{
			ScriptTexture2D* scriptTexture;
			ScriptResourceManager::instance().getScriptResource(icon, &scriptTexture, true);

			return scriptTexture->getManagedInstance();
		}

		return nullptr;
	}

	void ScriptWinPlatformInfo::internal_SetTaskbarIcon(ScriptWinPlatformInfo* thisPtr, ScriptTexture2D* texturePtr)
	{
		HTexture icon;

		if (texturePtr != nullptr)
			icon = texturePtr->getHandle();

		thisPtr->getWinPlatformInfo()->taskbarIcon = icon;
	}

	MonoString* ScriptWinPlatformInfo::internal_GetTitleText(ScriptWinPlatformInfo* thisPtr)
	{
		WString titleText = thisPtr->getWinPlatformInfo()->titlebarText;

		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), titleText);
	}

	void ScriptWinPlatformInfo::internal_SetTitleText(ScriptWinPlatformInfo* thisPtr, MonoString* text)
	{
		WString titleText = MonoUtil::monoToWString(text);

		thisPtr->getWinPlatformInfo()->titlebarText = titleText;
	}
}