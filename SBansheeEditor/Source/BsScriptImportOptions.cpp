#include "BsScriptImportOptions.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsTextureImportOptions.h"
#include "BsFontImportOptions.h"
#include "BsScriptCodeImportOptions.h"
#include "BsMeshImportOptions.h"
#include "BsRenderer.h"
#include "BsScriptFont.h"
#include "BsRTTIType.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	struct CharRange
	{
		UINT32 start, end;
	};

	ScriptImportOptionsBase::ScriptImportOptionsBase(MonoObject* instance)
		:ScriptObjectBase(instance)
	{ }

	void ScriptImportOptions::initRuntimeData() 
	{ }

	MonoObject* ScriptImportOptions::create(const SPtr<ImportOptions>& importOptions)
	{
		UINT32 typeId = importOptions->getRTTI()->getRTTIId();
		switch (typeId)
		{
		case TID_TextureImportOptions:
			return ScriptTextureImportOptions::create(std::static_pointer_cast<TextureImportOptions>(importOptions));
			break;
		case TID_FontImportOptions:
			return ScriptFontImportOptions::create(std::static_pointer_cast<FontImportOptions>(importOptions));
			break;
		case TID_ScriptCodeImportOptions:
			return ScriptScriptCodeImportOptions::create(std::static_pointer_cast<ScriptCodeImportOptions>(importOptions));
			break;
		}

		MonoObject* managedInstance = metaData.scriptClass->createInstance();
		ScriptImportOptions* scriptObj = ScriptImportOptions::toNative(managedInstance);
		scriptObj->mImportOptions = importOptions;

		return managedInstance;
	}

	ScriptImportOptions::ScriptImportOptions(MonoObject* instance)
		:ScriptObject(instance)
	{ 
		mImportOptions = bs_shared_ptr_new<ImportOptions>();
	}

	ScriptTextureImportOptions::ScriptTextureImportOptions(MonoObject* instance)
		:ScriptObject(instance)
	{
		mImportOptions = bs_shared_ptr_new<TextureImportOptions>();
	}

	void ScriptTextureImportOptions::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptTextureImportOptions::internal_CreateInstance);
		metaData.scriptClass->addInternalCall("Internal_GetPixelFormat", &ScriptTextureImportOptions::internal_GetPixelFormat);
		metaData.scriptClass->addInternalCall("Internal_SetPixelFormat", &ScriptTextureImportOptions::internal_SetPixelFormat);
		metaData.scriptClass->addInternalCall("Internal_GetGenerateMipmaps", &ScriptTextureImportOptions::internal_GetGenerateMipmaps);
		metaData.scriptClass->addInternalCall("Internal_SetGenerateMipmaps", &ScriptTextureImportOptions::internal_SetGenerateMipmaps);
		metaData.scriptClass->addInternalCall("Internal_GetMaxMipmapLevel", &ScriptTextureImportOptions::internal_GetMaxMipmapLevel);
		metaData.scriptClass->addInternalCall("Internal_SetMaxMipmapLevel", &ScriptTextureImportOptions::internal_SetMaxMipmapLevel);
		metaData.scriptClass->addInternalCall("Internal_GetCPUReadable", &ScriptTextureImportOptions::internal_GetCPUReadable);
		metaData.scriptClass->addInternalCall("Internal_SetCPUReadable", &ScriptTextureImportOptions::internal_SetCPUReadable);
		metaData.scriptClass->addInternalCall("Internal_GetIsSRGB", &ScriptTextureImportOptions::internal_GetIsSRGB);
		metaData.scriptClass->addInternalCall("Internal_SetIsSRGB", &ScriptTextureImportOptions::internal_SetIsSRGB);
	}

	SPtr<TextureImportOptions> ScriptTextureImportOptions::getTexImportOptions()
	{
		return std::static_pointer_cast<TextureImportOptions>(mImportOptions);
	}

	MonoObject* ScriptTextureImportOptions::create()
	{
		return metaData.scriptClass->createInstance();
	}

	MonoObject* ScriptTextureImportOptions::create(const SPtr<TextureImportOptions>& options)
	{
		MonoObject* managedInstance = metaData.scriptClass->createInstance();
		ScriptTextureImportOptions* scriptObj = ScriptTextureImportOptions::toNative(managedInstance);
		scriptObj->mImportOptions = options;

		return managedInstance;
	}

	void ScriptTextureImportOptions::internal_CreateInstance(MonoObject* instance)
	{
		ScriptTextureImportOptions* nativeInstance = new (bs_alloc<ScriptTextureImportOptions>()) ScriptTextureImportOptions(instance);
	}

	PixelFormat ScriptTextureImportOptions::internal_GetPixelFormat(ScriptTextureImportOptions* thisPtr)
	{
		return thisPtr->getTexImportOptions()->getFormat();
	}

	void ScriptTextureImportOptions::internal_SetPixelFormat(ScriptTextureImportOptions* thisPtr, PixelFormat value)
	{
		thisPtr->getTexImportOptions()->setFormat(value);
	}

	bool ScriptTextureImportOptions::internal_GetGenerateMipmaps(ScriptTextureImportOptions* thisPtr)
	{
		return thisPtr->getTexImportOptions()->getGenerateMipmaps();
	}

	void ScriptTextureImportOptions::internal_SetGenerateMipmaps(ScriptTextureImportOptions* thisPtr, bool value)
	{
		thisPtr->getTexImportOptions()->setGenerateMipmaps(value);
	}

	UINT32 ScriptTextureImportOptions::internal_GetMaxMipmapLevel(ScriptTextureImportOptions* thisPtr)
	{
		return thisPtr->getTexImportOptions()->getMaxMip();
	}

	void ScriptTextureImportOptions::internal_SetMaxMipmapLevel(ScriptTextureImportOptions* thisPtr, UINT32 value)
	{
		thisPtr->getTexImportOptions()->setMaxMip(value);
	}

	bool ScriptTextureImportOptions::internal_GetCPUReadable(ScriptTextureImportOptions* thisPtr)
	{
		return thisPtr->getTexImportOptions()->getCPUReadable();
	}

	void ScriptTextureImportOptions::internal_SetCPUReadable(ScriptTextureImportOptions* thisPtr, bool value)
	{
		thisPtr->getTexImportOptions()->setCPUReadable(value);
	}

	bool ScriptTextureImportOptions::internal_GetIsSRGB(ScriptTextureImportOptions* thisPtr)
	{
		return thisPtr->getTexImportOptions()->getSRGB();
	}

	void ScriptTextureImportOptions::internal_SetIsSRGB(ScriptTextureImportOptions* thisPtr, bool value)
	{
		thisPtr->getTexImportOptions()->setSRGB(value);
	}

	ScriptMeshImportOptions::ScriptMeshImportOptions(MonoObject* instance)
		:ScriptObject(instance)
	{
		mImportOptions = bs_shared_ptr_new<MeshImportOptions>();
	}

	void ScriptMeshImportOptions::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptMeshImportOptions::internal_CreateInstance);
		metaData.scriptClass->addInternalCall("Internal_GetCPUReadable", &ScriptMeshImportOptions::internal_GetCPUReadable);
		metaData.scriptClass->addInternalCall("Internal_SetCPUReadable", &ScriptMeshImportOptions::internal_SetCPUReadable);
		metaData.scriptClass->addInternalCall("Internal_GetImportNormals", &ScriptMeshImportOptions::internal_GetImportNormals);
		metaData.scriptClass->addInternalCall("Internal_SetImportNormals", &ScriptMeshImportOptions::internal_SetImportNormals);
		metaData.scriptClass->addInternalCall("Internal_GetImportTangents", &ScriptMeshImportOptions::internal_GetImportTangents);
		metaData.scriptClass->addInternalCall("Internal_SetImportTangents", &ScriptMeshImportOptions::internal_SetImportTangents);
		metaData.scriptClass->addInternalCall("Internal_GetImportSkin", &ScriptMeshImportOptions::internal_GetImportSkin);
		metaData.scriptClass->addInternalCall("Internal_SetImportSkin", &ScriptMeshImportOptions::internal_SetImportSkin);
		metaData.scriptClass->addInternalCall("Internal_GetImportAnimation", &ScriptMeshImportOptions::internal_GetImportAnimation);
		metaData.scriptClass->addInternalCall("Internal_SetImportAnimation", &ScriptMeshImportOptions::internal_SetImportAnimation);
		metaData.scriptClass->addInternalCall("Internal_GetImportBlendShapes", &ScriptMeshImportOptions::internal_GetImportBlendShapes);
		metaData.scriptClass->addInternalCall("Internal_SetImportBlendShapes", &ScriptMeshImportOptions::internal_SetImportBlendShapes);
		metaData.scriptClass->addInternalCall("Internal_GetScale", &ScriptMeshImportOptions::internal_GetScale);
		metaData.scriptClass->addInternalCall("Internal_SetScale", &ScriptMeshImportOptions::internal_SetScale);
	}

	SPtr<MeshImportOptions> ScriptMeshImportOptions::getMeshImportOptions()
	{
		return std::static_pointer_cast<MeshImportOptions>(mImportOptions);
	}

	MonoObject* ScriptMeshImportOptions::create()
	{
		return metaData.scriptClass->createInstance();
	}

	MonoObject* ScriptMeshImportOptions::create(const SPtr<MeshImportOptions>& options)
	{
		MonoObject* managedInstance = metaData.scriptClass->createInstance();
		ScriptMeshImportOptions* scriptObj = ScriptMeshImportOptions::toNative(managedInstance);
		scriptObj->mImportOptions = options;

		return managedInstance;
	}

	void ScriptMeshImportOptions::internal_CreateInstance(MonoObject* instance)
	{
		ScriptMeshImportOptions* nativeInstance = new (bs_alloc<ScriptMeshImportOptions>()) ScriptMeshImportOptions(instance);
	}

	bool ScriptMeshImportOptions::internal_GetCPUReadable(ScriptMeshImportOptions* thisPtr)
	{
		return thisPtr->getMeshImportOptions()->getCPUReadable();
	}

	void ScriptMeshImportOptions::internal_SetCPUReadable(ScriptMeshImportOptions* thisPtr, bool value)
	{
		thisPtr->getMeshImportOptions()->setCPUReadable(value);
	}

	bool ScriptMeshImportOptions::internal_GetImportNormals(ScriptMeshImportOptions* thisPtr)
	{
		return thisPtr->getMeshImportOptions()->getImportNormals();
	}

	void ScriptMeshImportOptions::internal_SetImportNormals(ScriptMeshImportOptions* thisPtr, bool value)
	{
		thisPtr->getMeshImportOptions()->setImportNormals(value);
	}

	bool ScriptMeshImportOptions::internal_GetImportTangents(ScriptMeshImportOptions* thisPtr)
	{
		return thisPtr->getMeshImportOptions()->getImportTangents();
	}

	void ScriptMeshImportOptions::internal_SetImportTangents(ScriptMeshImportOptions* thisPtr, bool value)
	{
		thisPtr->getMeshImportOptions()->setImportTangents(value);
	}

	bool ScriptMeshImportOptions::internal_GetImportSkin(ScriptMeshImportOptions* thisPtr)
	{
		return thisPtr->getMeshImportOptions()->getImportSkin();
	}

	void ScriptMeshImportOptions::internal_SetImportSkin(ScriptMeshImportOptions* thisPtr, bool value)
	{
		thisPtr->getMeshImportOptions()->setImportSkin(value);
	}

	bool ScriptMeshImportOptions::internal_GetImportAnimation(ScriptMeshImportOptions* thisPtr)
	{
		return thisPtr->getMeshImportOptions()->getImportAnimation();
	}

	void ScriptMeshImportOptions::internal_SetImportAnimation(ScriptMeshImportOptions* thisPtr, bool value)
	{
		thisPtr->getMeshImportOptions()->setImportAnimation(value);
	}

	bool ScriptMeshImportOptions::internal_GetImportBlendShapes(ScriptMeshImportOptions* thisPtr)
	{
		return thisPtr->getMeshImportOptions()->getImportBlendShapes();
	}

	void ScriptMeshImportOptions::internal_SetImportBlendShapes(ScriptMeshImportOptions* thisPtr, bool value)
	{
		thisPtr->getMeshImportOptions()->setImportBlendShapes(value);
	}

	float ScriptMeshImportOptions::internal_GetScale(ScriptMeshImportOptions* thisPtr)
	{
		return thisPtr->getMeshImportOptions()->getImportScale();
	}

	void ScriptMeshImportOptions::internal_SetScale(ScriptMeshImportOptions* thisPtr, float value)
	{
		thisPtr->getMeshImportOptions()->setImportScale(value);
	}

	ScriptFontImportOptions::ScriptFontImportOptions(MonoObject* instance)
		:ScriptObject(instance)
	{
		mImportOptions = bs_shared_ptr_new<FontImportOptions>();
	}

	void ScriptFontImportOptions::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptFontImportOptions::internal_CreateInstance);
		metaData.scriptClass->addInternalCall("Internal_GetFontSizes", &ScriptFontImportOptions::internal_GetFontSizes);
		metaData.scriptClass->addInternalCall("Internal_SetFontSizes", &ScriptFontImportOptions::internal_SetFontSizes);
		metaData.scriptClass->addInternalCall("Internal_GetDPI", &ScriptFontImportOptions::internal_GetDPI);
		metaData.scriptClass->addInternalCall("Internal_SetDPI", &ScriptFontImportOptions::internal_SetDPI);
		metaData.scriptClass->addInternalCall("Internal_GetAntialiasing", &ScriptFontImportOptions::internal_GetAntialiasing);
		metaData.scriptClass->addInternalCall("Internal_SetAntialiasing", &ScriptFontImportOptions::internal_SetAntialiasing);
		metaData.scriptClass->addInternalCall("Internal_GetCharRanges", &ScriptFontImportOptions::internal_GetCharRanges);
		metaData.scriptClass->addInternalCall("Internal_SetCharRanges", &ScriptFontImportOptions::internal_SetCharRanges);
	}

	SPtr<FontImportOptions> ScriptFontImportOptions::getFontImportOptions()
	{
		return std::static_pointer_cast<FontImportOptions>(mImportOptions);
	}

	MonoObject* ScriptFontImportOptions::create()
	{
		return metaData.scriptClass->createInstance();
	}

	MonoObject* ScriptFontImportOptions::create(const SPtr<FontImportOptions>& options)
	{
		MonoObject* managedInstance = metaData.scriptClass->createInstance();
		ScriptFontImportOptions* scriptObj = ScriptFontImportOptions::toNative(managedInstance);
		scriptObj->mImportOptions = options;

		return managedInstance;
	}

	void ScriptFontImportOptions::internal_CreateInstance(MonoObject* instance)
	{
		ScriptFontImportOptions* nativeInstance = new (bs_alloc<ScriptFontImportOptions>()) ScriptFontImportOptions(instance);
	}

	MonoArray* ScriptFontImportOptions::internal_GetFontSizes(ScriptFontImportOptions* thisPtr)
	{
		Vector<UINT32> fontSizes = thisPtr->getFontImportOptions()->getFontSizes();

		ScriptArray outArray = ScriptArray::create<UINT32>((UINT32)fontSizes.size());
		for (UINT32 i = 0; i < fontSizes.size(); i++)
			outArray.set(i, fontSizes[i]);

		return outArray.getInternal();
	}

	void ScriptFontImportOptions::internal_SetFontSizes(ScriptFontImportOptions* thisPtr, MonoArray* value)
	{
		ScriptArray inArray(value);

		Vector<UINT32> fontSizes(inArray.size());
		for (UINT32 i = 0; i < inArray.size(); i++)
			fontSizes[i] = inArray.get<UINT32>(i);

		thisPtr->getFontImportOptions()->setFontSizes(fontSizes);
	}

	UINT32 ScriptFontImportOptions::internal_GetDPI(ScriptFontImportOptions* thisPtr)
	{
		return thisPtr->getFontImportOptions()->getDPI();
	}

	void ScriptFontImportOptions::internal_SetDPI(ScriptFontImportOptions* thisPtr, UINT32 value)
	{
		thisPtr->getFontImportOptions()->setDPI(value);
	}

	bool ScriptFontImportOptions::internal_GetAntialiasing(ScriptFontImportOptions* thisPtr)
	{
		return thisPtr->getFontImportOptions()->getAntialiasing();
	}

	void ScriptFontImportOptions::internal_SetAntialiasing(ScriptFontImportOptions* thisPtr, bool value)
	{
		thisPtr->getFontImportOptions()->setAntialiasing(value);
	}

	MonoArray* ScriptFontImportOptions::internal_GetCharRanges(ScriptFontImportOptions* thisPtr)
	{
		Vector<std::pair<UINT32, UINT32>> charRanges = thisPtr->getFontImportOptions()->getCharIndexRanges();

		ScriptArray outArray = ScriptArray::create<ScriptCharRange>((UINT32)charRanges.size());
		for (UINT32 i = 0; i < (UINT32)charRanges.size(); i++)
		{
			CharRange range;
			range.start = charRanges[i].first;
			range.end = charRanges[i].second;
			outArray.set(i, range);
		}

		return outArray.getInternal();
	}

	void ScriptFontImportOptions::internal_SetCharRanges(ScriptFontImportOptions* thisPtr, MonoArray* value)
	{
		ScriptArray inArray(value);

		thisPtr->getFontImportOptions()->clearCharIndexRanges();
		for (UINT32 i = 0; i < inArray.size(); i++)
		{
			CharRange range = inArray.get<CharRange>(i);
			thisPtr->getFontImportOptions()->addCharIndexRange(range.start, range.end);
		}
	}

	ScriptScriptCodeImportOptions::ScriptScriptCodeImportOptions(MonoObject* instance)
		:ScriptObject(instance)
	{
		mImportOptions = bs_shared_ptr_new<ScriptCodeImportOptions>();
	}

	void ScriptScriptCodeImportOptions::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptScriptCodeImportOptions::internal_CreateInstance);
		metaData.scriptClass->addInternalCall("Internal_IsEditorScript", &ScriptScriptCodeImportOptions::internal_IsEditorScript);
		metaData.scriptClass->addInternalCall("Internal_SetEditorScript", &ScriptScriptCodeImportOptions::internal_SetEditorScript);
	}

	SPtr<ScriptCodeImportOptions> ScriptScriptCodeImportOptions::getCodeImportOptions()
	{
		return std::static_pointer_cast<ScriptCodeImportOptions>(mImportOptions);
	}

	MonoObject* ScriptScriptCodeImportOptions::create()
	{
		return metaData.scriptClass->createInstance();
	}

	MonoObject* ScriptScriptCodeImportOptions::create(const SPtr<ScriptCodeImportOptions>& options)
	{
		MonoObject* managedInstance = metaData.scriptClass->createInstance();
		ScriptScriptCodeImportOptions* scriptObj = ScriptScriptCodeImportOptions::toNative(managedInstance);
		scriptObj->mImportOptions = options;

		return managedInstance;
	}

	void ScriptScriptCodeImportOptions::internal_CreateInstance(MonoObject* instance)
	{
		ScriptScriptCodeImportOptions* nativeInstance = new (bs_alloc<ScriptScriptCodeImportOptions>()) ScriptScriptCodeImportOptions(instance);
	}

	bool ScriptScriptCodeImportOptions::internal_IsEditorScript(ScriptScriptCodeImportOptions* thisPtr)
	{
		return thisPtr->getCodeImportOptions()->isEditorScript();
	}

	void ScriptScriptCodeImportOptions::internal_SetEditorScript(ScriptScriptCodeImportOptions* thisPtr, bool value)
	{
		thisPtr->getCodeImportOptions()->setEditorScript(value);
	}
}