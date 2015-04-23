#include "BsScriptResource.h"

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
		case TID_PlainText:
			return ScriptResourceType::PlainText;
		case TID_ScriptCode:
			return ScriptResourceType::ScriptCode;
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
		case ScriptResourceType::PlainText:
			return TID_PlainText;
		case ScriptResourceType::ScriptCode:
			return TID_ScriptCode;
		}

		return 0;
	}
}