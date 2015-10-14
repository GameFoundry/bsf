#include "BsScriptResource.h"
#include "BsScriptResourceManager.h"
#include "BsResource.h"
#include "BsMonoUtil.h"

namespace BansheeEngine
{
	ScriptResourceBase::ScriptResourceBase(MonoObject* instance)
		:PersistentScriptObjectBase(instance), mRefreshInProgress(false)
	{ }

	ScriptObjectBackup ScriptResourceBase::beginRefresh()
	{
		mRefreshInProgress = true;

		return PersistentScriptObjectBase::beginRefresh();
	}

	void ScriptResourceBase::endRefresh(const ScriptObjectBackup& backupData)
	{
		mRefreshInProgress = false;

		PersistentScriptObjectBase::endRefresh(backupData);
	}

	void ScriptResourceBase::destroy()
	{
		mManagedInstance = nullptr;

		if (!mRefreshInProgress)
			ScriptResourceManager::instance().destroyScriptResource(this);
	}

	void ScriptResource::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetName", &ScriptResource::internal_getName);
		metaData.scriptClass->addInternalCall("Internal_GetUUID", &ScriptResource::internal_getUUID);
	}

	ScriptResourceType ScriptResource::getTypeFromTypeId(UINT32 typeId)
	{
		switch (typeId)
		{
		case TID_Texture:
			return ScriptResourceType::Texture;
		case TID_SpriteTexture:
			return ScriptResourceType::SpriteTexture;
		case TID_Mesh:
			return ScriptResourceType::Mesh;
		case TID_Shader:
			return ScriptResourceType::Shader;
		case TID_Material:
			return ScriptResourceType::Material;
		case TID_Font:
			return ScriptResourceType::Font;
		case TID_Prefab:
			return ScriptResourceType::Prefab;
		case TID_StringTable:
			return ScriptResourceType::StringTable;
		case TID_PlainText:
			return ScriptResourceType::PlainText;
		case TID_ScriptCode:
			return ScriptResourceType::ScriptCode;
		case TID_GUISkin:
			return ScriptResourceType::GUISkin;
		}

		return ScriptResourceType::Undefined;
	}

	UINT32 ScriptResource::getTypeIdFromType(ScriptResourceType type)
	{
		switch (type)
		{
		case ScriptResourceType::Texture:
			return TID_Texture;
		case ScriptResourceType::SpriteTexture:
			return TID_SpriteTexture;
		case ScriptResourceType::Mesh:
			return TID_Mesh;
		case ScriptResourceType::Shader:
			return TID_Shader;
		case ScriptResourceType::Font:
			return TID_Font;
		case ScriptResourceType::Material:
			return TID_Material;
		case ScriptResourceType::Prefab:
			return TID_Prefab;
		case ScriptResourceType::StringTable:
			return TID_StringTable;
		case ScriptResourceType::PlainText:
			return TID_PlainText;
		case ScriptResourceType::ScriptCode:
			return TID_ScriptCode;
		case ScriptResourceType::GUISkin:
			return TID_GUISkin;
		}

		return 0;
	}

	MonoString* ScriptResource::internal_getName(ScriptResourceBase* nativeInstance)
	{
		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), nativeInstance->getGenericHandle()->getName());
	}

	MonoString* ScriptResource::internal_getUUID(ScriptResourceBase* nativeInstance)
	{
		return MonoUtil::stringToMono(MonoManager::instance().getDomain(), nativeInstance->getGenericHandle().getUUID());
	}
}