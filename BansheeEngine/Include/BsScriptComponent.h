#pragma once

#include "BsPrerequisites.h"
#include "CmComponent.h"
#include "BsScriptManager.h"

namespace BansheeEngine
{
	class BS_EXPORT ScriptComponent : CM::Component
	{
	public:
		virtual ~ScriptComponent() {}

	protected:
		friend class CM::SceneObject;

		ScriptComponent(const CM::HSceneObject& parent, MonoObject* managedInstance);

		virtual void update();

		MonoObject* mManagedInstance;
		std::function<void()> mUpdateThunk;
	};
}