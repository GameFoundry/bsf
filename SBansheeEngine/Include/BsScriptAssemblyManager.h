#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsManagedSerializableObjectInfo.h"
#include "BsModule.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptAssemblyManager : public Module<ScriptAssemblyManager>
	{
	public:
		ScriptAssemblyManager();
		~ScriptAssemblyManager();

		void loadAssemblyInfo(const String& assemblyName);
		void refreshAssemblyInfo();
		bool getSerializableObjectInfo(const String& ns, const String& typeName, std::shared_ptr<ManagedSerializableObjectInfo>& outInfo);
		bool hasSerializableObjectInfo(const String& ns, const String& typeName);

		Vector<String> getScriptAssemblies() const;

		MonoClass* getSystemArrayClass() const { return mSystemArrayClass; }
		MonoClass* getSystemGenericListClass() const { return mSystemGenericListClass; }
		MonoClass* getSystemGenericDictionaryClass() const { return mSystemGenericDictionaryClass; }
		MonoClass* getComponentClass() const { return mComponentClass; }
		MonoClass* getMissingComponentClass() const { return mMissingComponentClass; }
		MonoClass* getSceneObjectClass() const { return mSceneObjectClass; }
		MonoClass* getManagedResourceClass() const { return mManagedResourceClass; }
		MonoClass* getTexture2DClass() const { return mTexture2DClass; }
		MonoClass* getTexture3DClass() const { return mTexture3DClass; }
		MonoClass* getTextureCubeClass() const { return mTextureCubeClass; }
		MonoClass* getSpriteTextureClass() const { return mSpriteTextureClass; }
		MonoClass* getShaderClass() const { return mShaderClass; }
		MonoClass* getMaterialClass() const { return mMaterialClass; }
		MonoClass* getMeshClass() const { return mMeshClass; }
		MonoClass* getFontClass() const { return mFontClass; }
		MonoClass* getPrefabClass() const { return mPrefabClass;  }
		MonoClass* getPlainTextClass() const { return mPlainTextClass; }
		MonoClass* getScriptCodeClass() const { return mScriptCodeClass; }

		ManagedSerializableTypeInfoPtr determineType(MonoClass* monoClass);
	private:
		UnorderedMap<String, std::shared_ptr<ManagedSerializableAssemblyInfo>> mAssemblyInfos;
		bool mBaseTypesInitialized;

		MonoClass* mSystemArrayClass;
		MonoClass* mSystemGenericListClass;
		MonoClass* mSystemGenericDictionaryClass;

		MonoClass* mComponentClass;
		MonoClass* mSceneObjectClass;
		MonoClass* mMissingComponentClass;

		MonoClass* mTexture2DClass;
		MonoClass* mTexture3DClass;
		MonoClass* mTextureCubeClass;
		MonoClass* mSpriteTextureClass;
		MonoClass* mShaderClass;
		MonoClass* mMaterialClass;
		MonoClass* mMeshClass;
		MonoClass* mManagedResourceClass;
		MonoClass* mFontClass;
		MonoClass* mPrefabClass;
		MonoClass* mPlainTextClass;
		MonoClass* mScriptCodeClass;

		MonoClass* mSerializeObjectAttribute;
		MonoClass* mDontSerializeFieldAttribute;
		MonoClass* mSerializeFieldAttribute;
		MonoClass* mHideInInspectorAttribute;

		void clearScriptObjects();

		void initializeBaseTypes();
	};
}