#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGameObject.h"
#include "BsScriptObject.h"
#include "CmFont.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptComponent : public ScriptGameObject, public ScriptObject<ScriptComponent>
	{
	public:
		static void initMetaData();

		virtual CM::HGameObject getNativeHandle() const { return mManagedComponent; }
		virtual void setNativeHandle(const CM::HGameObject& gameObject);

	private:
		static void internal_createInstance(MonoObject* instance, MonoString* ns, MonoString* typeName);
		static void internal_destroyInstance(ScriptComponent* nativeInstance);

		static void initRuntimeData();

		ScriptComponent(const CM::GameObjectHandle<ManagedComponent>& managedComponent);

		CM::GameObjectHandle<ManagedComponent> mManagedComponent;
	};
}