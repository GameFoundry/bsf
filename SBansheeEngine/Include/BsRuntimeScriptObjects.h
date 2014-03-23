#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptSerializableObjectInfo.h"
#include "CmModule.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT RuntimeScriptObjects : public CM::Module<RuntimeScriptObjects>
	{
	public:
		RuntimeScriptObjects();
		~RuntimeScriptObjects();

		void refreshScriptObjects(const CM::String& assemblyName);
		bool getSerializableObjectInfo(const CM::String& ns, const CM::String& typeName, std::shared_ptr<ScriptSerializableObjectInfo>& outInfo);
		bool hasSerializableObjectInfo(const CM::String& ns, const CM::String& typeName);

		bool isArray(MonoObject* object);

		MonoClass* getSystemArrayClass() const { return mSystemArrayClass; }
		MonoClass* getComponentClass() const { return mComponentClass; }
		MonoClass* getSceneObjectClass() const { return mSceneObjectClass; }
		MonoClass* getTextureClass() const { return mTextureClass; }
		MonoClass* getSpriteTextureClass() const { return mSpriteTextureClass; }
	private:
		CM::UnorderedMap<CM::String, std::shared_ptr<ScriptSerializableAssemblyInfo>>::type mAssemblyInfos;
		bool mBaseTypesInitialized;

		MonoClass* mSystemArrayClass;

		MonoClass* mComponentClass;
		MonoClass* mSceneObjectClass;

		MonoClass* mTextureClass;
		MonoClass* mSpriteTextureClass;

		MonoClass* mSerializableObjectAttribute;
		MonoClass* mDontSerializeFieldAttribute;
		MonoClass* mSerializeFieldAttribute;
		MonoClass* mHideInInspectorAttribute;

		void clearScriptObjects(const CM::String& assemblyName);

		void initializeBaseTypes();
		ScriptSerializableTypeInfoPtr determineType(MonoClass* monoClass);
	};
}