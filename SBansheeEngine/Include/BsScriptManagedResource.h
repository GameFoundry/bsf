#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptManagedResource : public ScriptObject<ScriptManagedResource, ScriptResourceBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ManagedResource")

		ScriptManagedResource(MonoObject* instance, const HManagedResource& resource);

		HResource getNativeHandle() const { return mResource; }
		void setNativeHandle(const HResource& resource);
	private:
		friend class ScriptResourceManager;

		static void internal_createInstance(MonoObject* instance);

		virtual ScriptObjectBackup beginRefresh();
		virtual void endRefresh(const ScriptObjectBackup& backupData);
		virtual MonoObject* _createManagedInstance(bool construct);
		void _onManagedInstanceDeleted();

		HManagedResource mResource;
		String mNamespace;
		String mType;
	};
}