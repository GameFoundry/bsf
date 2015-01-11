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
			MonoClass* componentType;
			MonoMethod* drawGizmosMethod;
			UINT32 flags;
		};

	public:
		ScriptGizmoManager(ScriptAssemblyManager& scriptObjectManager);
		~ScriptGizmoManager();

		void update();

	private:
		void reloadAssemblyData();
		bool isValidDrawGizmoMethod(MonoMethod* method, MonoClass*& componentType, UINT32& drawGizmoFlags);

		ScriptAssemblyManager& mScriptObjectManager;
		HEvent mDomainLoadedConn;

		MonoClass* mDrawGizmoAttribute;
		MonoField* mFlagsField;
		Map<String, GizmoData> mGizmoDrawers;
	};
}