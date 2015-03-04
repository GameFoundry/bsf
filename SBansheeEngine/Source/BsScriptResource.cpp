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
		case TID_GpuProgram:
			return ScriptResourceType::GpuProgram;
		case TID_Font:
			return ScriptResourceType::Font;
		}

		return ScriptResourceType::Undefined;
	}
}