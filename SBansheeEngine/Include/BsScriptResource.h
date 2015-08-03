#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	enum class ScriptResourceType
	{
		Texture, SpriteTexture, Mesh, Font, Shader, Material, Prefab, PlainText, ScriptCode, StringTable, Undefined
	};

	class BS_SCR_BE_EXPORT ScriptResourceBase : public PersistentScriptObjectBase
	{
	public:
		virtual HResource getNativeHandle() const = 0;
		virtual void setNativeHandle(const HResource& resource) = 0;

		virtual ScriptObjectBackup beginRefresh();
		virtual void endRefresh(const ScriptObjectBackup& backupData);

	protected:
		friend class ScriptResourceManager;

		ScriptResourceBase(MonoObject* instance);
		virtual ~ScriptResourceBase() {}

		bool mRefreshInProgress;
	};

	class BS_SCR_BE_EXPORT ScriptResource : public ScriptObject<ScriptResource, ScriptResourceBase>
	{
	public:
		static String getAssemblyName() { return ENGINE_ASSEMBLY; }
		static String getNamespace() { return "BansheeEngine"; }
		static String getTypeName() { return "Resource"; }
		static void initRuntimeData() { }

		static ScriptResourceType getTypeFromTypeId(UINT32 typeId);
		static UINT32 getTypeIdFromType(ScriptResourceType type);
	private:
		ScriptResource(MonoObject* instance)
			:ScriptObject(instance)
		{ }
	};
}