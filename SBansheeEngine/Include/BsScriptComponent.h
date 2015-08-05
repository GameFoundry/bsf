#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGameObject.h"
#include "BsScriptObject.h"
#include "BsFont.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for ManagedComponent.
	 */
	class BS_SCR_BE_EXPORT ScriptComponent : public ScriptObject<ScriptComponent, ScriptGameObjectBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Component")

		/**
		 * @copydoc	ScriptGameObjectBase::getNativeHandle
		 */
		virtual HGameObject getNativeHandle() const override { return mManagedComponent; }

		/**
		 * @copydoc	ScriptGameObjectBase::setNativeHandle
		 */
		virtual void setNativeHandle(const HGameObject& gameObject) override;

	private:
		friend class ScriptGameObjectManager;

		ScriptComponent(MonoObject* instance);
		~ScriptComponent() {}

		/**
		 * @copydoc	ScriptObjectBase::beginRefresh
		 */
		virtual ScriptObjectBackup beginRefresh() override;

		/**
		 * @copydoc	ScriptObjectBase::endRefresh
		 */
		virtual void endRefresh(const ScriptObjectBackup& backupData) override;

		/**
		 * @copydoc	ScriptObjectBase::_createManagedInstance
		 */
		virtual MonoObject* _createManagedInstance(bool construct) override;

		/**
		 * @copydoc	ScriptObjectBase::_onManagedInstanceDeleted
		 */
		void _onManagedInstanceDeleted() override;

		/**
		 * @brief	Checks if the provided game object is destroyed and logs
		 *			a warning if it is.
		 */
		static bool checkIfDestroyed(const GameObjectHandleBase& handle);

		GameObjectHandle<ManagedComponent> mManagedComponent;
		String mNamespace;
		String mType;
		bool mTypeMissing;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static MonoObject* internal_addComponent(MonoObject* parentSceneObject, MonoReflectionType* type);
		static MonoObject* internal_getComponent(MonoObject* parentSceneObject, MonoReflectionType* type);
		static MonoArray* internal_getComponents(MonoObject* parentSceneObject);
		static void internal_removeComponent(MonoObject* parentSceneObject, MonoReflectionType* type);
		static MonoObject* internal_getSceneObject(ScriptComponent* nativeInstance);
	};
}