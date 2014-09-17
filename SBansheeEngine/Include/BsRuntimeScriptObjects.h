#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsManagedSerializableObjectInfo.h"
#include "BsModule.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT RuntimeScriptObjects : public Module<RuntimeScriptObjects>
	{
	public:
		RuntimeScriptObjects();
		~RuntimeScriptObjects();

		void refreshScriptObjects(const String& assemblyName);
		bool getSerializableObjectInfo(const String& ns, const String& typeName, std::shared_ptr<ManagedSerializableObjectInfo>& outInfo);
		bool hasSerializableObjectInfo(const String& ns, const String& typeName);

		Vector<String> getInitializedAssemblies() const;

		MonoClass* getSystemArrayClass() const { return mSystemArrayClass; }
		MonoClass* getSystemGenericListClass() const { return mSystemGenericListClass; }
		MonoClass* getSystemGenericDictionaryClass() const { return mSystemGenericDictionaryClass; }
		MonoClass* getComponentClass() const { return mComponentClass; }
		MonoClass* getSceneObjectClass() const { return mSceneObjectClass; }
		MonoClass* getManagedResourceClass() const { return mManagedResourceClass; }
		MonoClass* getTextureClass() const { return mTextureClass; }
		MonoClass* getSpriteTextureClass() const { return mSpriteTextureClass; }

		ManagedSerializableTypeInfoPtr determineType(MonoClass* monoClass);

		Event<void(MonoAssembly*)> onAssemblyRefreshed;
	private:
		UnorderedMap<String, std::shared_ptr<ManagedSerializableAssemblyInfo>> mAssemblyInfos;
		bool mBaseTypesInitialized;

		MonoClass* mSystemArrayClass;
		MonoClass* mSystemGenericListClass;
		MonoClass* mSystemGenericDictionaryClass;

		MonoClass* mComponentClass;
		MonoClass* mSceneObjectClass;

		MonoClass* mTextureClass;
		MonoClass* mSpriteTextureClass;
		MonoClass* mManagedResourceClass;

		MonoClass* mSerializeObjectAttribute;
		MonoClass* mDontSerializeFieldAttribute;
		MonoClass* mSerializeFieldAttribute;
		MonoClass* mHideInInspectorAttribute;

		void clearScriptObjects(const String& assemblyName);

		void initializeBaseTypes();
	};
}