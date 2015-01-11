#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsComponent.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	struct ComponentBackupData;

	class BS_SCR_BE_EXPORT ManagedComponent : public Component
	{
	public:
		~ManagedComponent();

		MonoObject* getManagedInstance() const { return mManagedInstance; }
		MonoReflectionType* getRuntimeType() const { return mRuntimeType; }

		const String& getManagedNamespace() const { return mNamespace; }
		const String& getManagedTypeName() const { return mTypeName; }
		const String& getManagedFullTypeName() const { return mFullTypeName; }

		ComponentBackupData backup(bool clearExisting = true);
		void restore(MonoObject* instance, const ComponentBackupData& data);

		void triggerOnReset();

	private:
		typedef void(__stdcall *OnInitializedThunkDef) (MonoObject*, MonoException**);
		typedef void(__stdcall *UpdateThunkDef) (MonoObject*, MonoException**);
		typedef void(__stdcall *OnDestroyedThunkDef) (MonoObject*, MonoException**);
		typedef void(__stdcall *OnResetThunkDef) (MonoObject*, MonoException**);

		MonoObject* mManagedInstance;
		MonoReflectionType* mRuntimeType;
		uint32_t mManagedHandle;

		String mNamespace;
		String mTypeName;
		String mFullTypeName;

		OnInitializedThunkDef mOnInitializedThunk;
		UpdateThunkDef mUpdateThunk;
		OnResetThunkDef mOnResetThunk;
		OnDestroyedThunkDef mOnDestroyThunk;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	protected:
		friend class SceneObject;
		friend class ScriptComponent;

		/** Standard constructor.
        */
		ManagedComponent(const HSceneObject& parent, MonoReflectionType* runtimeType);
		void initialize(MonoObject* object);

		void onInitialized();
		void onDestroyed();

	public:
		virtual void update();

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedComponentRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

	protected:
		ManagedComponent(); // Serialization only
	};

	struct ComponentBackupData
	{
		struct DataBlock
		{
			UINT8* data;
			UINT32 size;
		};

		DataBlock mTypeInfo;
		DataBlock mObjectData;
	};
}