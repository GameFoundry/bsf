#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Types of resources accessible from script code.
	 */
	// Note: Must be the same as C# enum ScriptResourceType
	enum class ScriptResourceType
	{
		Texture, SpriteTexture, Mesh, Font, Shader, Material, Prefab, PlainText, ScriptCode, StringTable, Undefined
	};

	/**
	 * @brief	Base class for all resource interop objects.
	 */
	class BS_SCR_BE_EXPORT ScriptResourceBase : public PersistentScriptObjectBase
	{
	public:
		/**
		 * @brief	Returns the internal wrapped resource.
		 */
		virtual HResource getNativeHandle() const = 0;

		/**
		 * @brief	Sets the internal resource this object wraps.
		 */
		virtual void setNativeHandle(const HResource& resource) = 0;

		/**
		 * @copydoc	ScriptObjectBase::beginRefresh
		 */
		virtual ScriptObjectBackup beginRefresh() override;

		/**
		 * @copydoc	ScriptObjectBase::endRefresh
		 */
		virtual void endRefresh(const ScriptObjectBackup& backupData) override;

	protected:
		friend class ScriptResourceManager;

		ScriptResourceBase(MonoObject* instance);
		virtual ~ScriptResourceBase() {}

		bool mRefreshInProgress;
	};

	/**
	 * @brief	Interop class between C++ & CLR for Resource.
	 */
	class BS_SCR_BE_EXPORT ScriptResource : public ScriptObject<ScriptResource, ScriptResourceBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Resource")

		/**
		 * @brief	Converts a RTTI id belonging to a resource type into a ScriptResourceType.
		 */
		static ScriptResourceType getTypeFromTypeId(UINT32 typeId);

		/**
		 * @brief	Converts a ScriptResourceType into a RTTI id belonging to that resource type.
		 */
		static UINT32 getTypeIdFromType(ScriptResourceType type);
	private:
		ScriptResource(MonoObject* instance)
			:ScriptObject(instance)
		{ }
	};
}