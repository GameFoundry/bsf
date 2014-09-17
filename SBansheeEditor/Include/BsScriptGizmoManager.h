#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	enum class DrawGizmoFlags
	{
		Selected = 0x01,
		ParentSelected = 0x02,
		NotSelected = 0x04,
		Pickable = 0x08
	};

	class BS_SCR_BED_EXPORT ScriptGizmoManager : public Module<ScriptGizmoManager>
	{
		struct GizmoData
		{
			UINT32 assemblyId;
			MonoClass* componentType;
			MonoMethod* drawGizmosMethod;
			UINT32 flags;
		};

	public:
		ScriptGizmoManager(RuntimeScriptObjects& scriptObjectManager);
		~ScriptGizmoManager();

		void update();

	private:
		void reloadAssemblyMethods(MonoAssembly* assembly);
		bool isValidDrawGizmoMethod(MonoMethod* method, MonoClass*& componentType, UINT32& drawGizmoFlags);

		RuntimeScriptObjects& mScriptObjectManager;
		HEvent mAssemblyRefreshedConn;

		MonoClass* mDrawGizmoAttribute;
		MonoField* mFlagsField;
		Map<String, UINT32> mAssemblyNameToId;
		Map<String, GizmoData> mGizmoDrawers;
		UINT32 mNextAssemblyId;

		Vector<MonoAssembly*> mDelayedLoad;
	};
}