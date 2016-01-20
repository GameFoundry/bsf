//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsComponent.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	struct ComponentBackupData;

	/**
	 * @brief	Component that internally wraps a managed component object
	 *			that can be of user-defined type. Acts as interop interop layer
	 *			between native Component and a managed user defined component type
	 *			since managed types cannot simply derive from Component
	 *			to implement its functionality.
	 */
	class BS_SCR_BE_EXPORT ManagedComponent : public Component
	{
	public:
		~ManagedComponent();

		/**
		 * @brief	Returns managed component object instance.
		 */
		MonoObject* getManagedInstance() const { return mManagedInstance; }

		/**
		 * @brief	Returns managed type of the component.
		 */
		MonoReflectionType* getRuntimeType() const { return mRuntimeType; }

		/**
		 * @brief	Returns namespace of the managed component.
		 */
		const String& getManagedNamespace() const { return mNamespace; }

		/**
		 * @brief	Returns type name of the managed component.
		 */
		const String& getManagedTypeName() const { return mTypeName; }

		/**
		 * @brief	Returns namespace and type name of the component in format "namespace.typename".
		 */
		const String& getManagedFullTypeName() const { return mFullTypeName; }

		/**
		 * @brief	Serializes the internal managed component.
		 *
		 * @param	clearExisting	Should the managed component handle be released. (Will trigger a finalizer
		 *							if this is the last reference to it)
		 * 
		 * @return	An object containing the serialized component. You can provide this to ::restore
		 *			method to re-create the original component.
		 */
		ComponentBackupData backup(bool clearExisting = true);

		/**
		 * @brief	Restores a component from previously serialized data.
		 *
		 * @param	instance	New instance of the managed component. Must be of the valid component type
		 *						or of BansheeEngine.MissingComponent type if the original type is missing.
		 * @param	data		Serialized managed component data that will be used for initializing
		 *						the new managed instance.
		 * @param	missingType	Is the component's type missing (can happen after assembly reload).
		 *						If true then the serialized data will be stored internally until later
		 *						date when user perhaps restores the type with another refresh.
		 *						\p instance must be null if this is true.
		 */
		void restore(MonoObject* instance, const ComponentBackupData& data, bool missingType);

		/**
		 * @brief	Triggers the managed OnInitialize callback.
		 */
		void triggerOnInitialize();

		/**
		 * @brief	Triggers the managed OnReset callback.
		 */
		void triggerOnReset();

	private:
		/**
		 * @brief	Finalizes construction of the object. Must be called before use or when
		 *			the managed component instance changes.
		 *
		 * @param	object	Managed component instance.
		 */
		void initialize(MonoObject* object);

		typedef void(__stdcall *OnInitializedThunkDef) (MonoObject*, MonoException**);
		typedef void(__stdcall *OnUpdateThunkDef) (MonoObject*, MonoException**);
		typedef void(__stdcall *OnDestroyedThunkDef) (MonoObject*, MonoException**);
		typedef void(__stdcall *OnResetThunkDef) (MonoObject*, MonoException**);
		typedef void(__stdcall *OnEnabledThunkDef) (MonoObject*, MonoException**);
		typedef void(__stdcall *OnDisabledThunkDef) (MonoObject*, MonoException**);

		MonoObject* mManagedInstance;
		MonoReflectionType* mRuntimeType;
		uint32_t mManagedHandle;

		String mNamespace;
		String mTypeName;
		String mFullTypeName;
		bool mRunInEditor;
		bool mRequiresReset;

		bool mMissingType;
		ManagedSerializableObjectPtr mSerializedObjectData;
		ManagedSerializableObjectInfoPtr mObjInfo; // Transient

		OnInitializedThunkDef mOnInitializedThunk;
		OnUpdateThunkDef mOnUpdateThunk;
		OnResetThunkDef mOnResetThunk;
		OnDestroyedThunkDef mOnDestroyThunk;
		OnDestroyedThunkDef mOnDisabledThunk;
		OnDestroyedThunkDef mOnEnabledThunk;
		MonoMethod* mCalculateBoundsMethod;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	protected:
		friend class SceneObject;
		friend class ScriptComponent;

		ManagedComponent(const HSceneObject& parent, MonoReflectionType* runtimeType);

		/**
		 * @copydoc	Component::instantiate
		 */
		void instantiate() override;

		/**
		 * @copydoc	Component::onInitialized
		 */
		void onInitialized() override;

		/**
		 * @copydoc	Component::onDestroyed
		 */
		void onDestroyed() override;

		/**
		 * @copydoc	Component::onEnabled 
		 */
		void onEnabled() override;

		/**
		 * @copydoc	Component::onDisabled 
		 */
		void onDisabled() override;

	public:
		/**
		 * @copydoc	Component::update
		 */
		void update() override;

		/**
		 * @copydoc	Component::typeEquals
		 */
		bool typeEquals(const Component& other) override;

		/**
		 * @copydoc	Component::calculateBounds
		 */
		bool calculateBounds(Bounds& bounds) override;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedComponentRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;

	protected:
		ManagedComponent(); // Serialization only
	};

	/**
	 * @brief	Contains serialized component data buffer.
	 */
	struct ComponentBackupData
	{
		UINT8* data;
		UINT32 size;
	};
}