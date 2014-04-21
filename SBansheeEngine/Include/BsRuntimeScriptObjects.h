#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsManagedSerializableObjectInfo.h"
#include "CmModule.h"
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

		MonoClass* getSystemArrayClass() const { return mSystemArrayClass; }
		MonoClass* getSystemGenericListClass() const { return mSystemGenericListClass; }
		MonoClass* getSystemGenericDictionaryClass() const { return mSystemGenericDictionaryClass; }
		MonoClass* getComponentClass() const { return mComponentClass; }
		MonoClass* getSceneObjectClass() const { return mSceneObjectClass; }
		MonoClass* getTextureClass() const { return mTextureClass; }
		MonoClass* getSpriteTextureClass() const { return mSpriteTextureClass; }
	private:
		UnorderedMap<String, std::shared_ptr<ManagedSerializableAssemblyInfo>>::type mAssemblyInfos;
		bool mBaseTypesInitialized;

		MonoClass* mSystemArrayClass;
		MonoClass* mSystemGenericListClass;
		MonoClass* mSystemGenericDictionaryClass;

		MonoClass* mComponentClass;
		MonoClass* mSceneObjectClass;

		MonoClass* mTextureClass;
		MonoClass* mSpriteTextureClass;

		MonoClass* mSerializeObjectAttribute;
		MonoClass* mDontSerializeFieldAttribute;
		MonoClass* mSerializeFieldAttribute;
		MonoClass* mHideInInspectorAttribute;

		void clearScriptObjects(const String& assemblyName);

		void initializeBaseTypes();
		ManagedSerializableTypeInfoPtr determineType(MonoClass* monoClass);
	};
}