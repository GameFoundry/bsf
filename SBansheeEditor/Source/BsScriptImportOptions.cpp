#include "BsScriptImportOptions.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsTextureImportOptions.h"
#include "BsFontImportOptions.h"
#include "BsGpuProgramImportOptions.h"
#include "BsScriptCodeImportOptions.h"
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
		case TID_GpuProgramImportOptions:
			return ScriptGpuProgramImportOptions::create(std::static_pointer_cast<GpuProgramImportOptions>(importOptions));
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
		mImportOptions = bs_shared_ptr<ImportOptions>();
	}

	ScriptTextureImportOptions::ScriptTextureImportOptions(MonoObject* instance)
		:ScriptObject(instance)
	{
		mImportOptions = bs_shared_ptr<TextureImportOptions>();
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

	ScriptFontImportOptions::ScriptFontImportOptions(MonoObject* instance)
		:ScriptObject(instance)
	{
		mImportOptions = bs_shared_ptr<FontImportOptions>();
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

	ScriptGpuProgramImportOptions::ScriptGpuProgramImportOptions(MonoObject* instance)
		:ScriptObject(instance)
	{
		mImportOptions = bs_shared_ptr<GpuProgramImportOptions>();
	}

	void ScriptGpuProgramImportOptions::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGpuProgramImportOptions::internal_CreateInstance);
		metaData.scriptClass->addInternalCall("Internal_GetEntryPoint", &ScriptGpuProgramImportOptions::internal_GetEntryPoint);
		metaData.scriptClass->addInternalCall("Internal_SetEntryPoint", &ScriptGpuProgramImportOptions::internal_SetEntryPoint);
		metaData.scriptClass->addInternalCall("Internal_GetLanguage", &ScriptGpuProgramImportOptions::internal_GetLanguage);
		metaData.scriptClass->addInternalCall("Internal_SetLanguage", &ScriptGpuProgramImportOptions::internal_SetLanguage);
		metaData.scriptClass->addInternalCall("Internal_GetProfile", &ScriptGpuProgramImportOptions::internal_GetProfile);
		metaData.scriptClass->addInternalCall("Internal_SetProfile", &ScriptGpuProgramImportOptions::internal_SetProfile);
		metaData.scriptClass->addInternalCall("Internal_GetType", &ScriptGpuProgramImportOptions::internal_GetType);
		metaData.scriptClass->addInternalCall("Internal_SetType", &ScriptGpuProgramImportOptions::internal_SetType);
	}

	SPtr<GpuProgramImportOptions> ScriptGpuProgramImportOptions::getGpuProgImportOptions()
	{
		return std::static_pointer_cast<GpuProgramImportOptions>(mImportOptions);
	}

	MonoObject* ScriptGpuProgramImportOptions::create()
	{
		return metaData.scriptClass->createInstance();
	}

	MonoObject* ScriptGpuProgramImportOptions::create(const SPtr<GpuProgramImportOptions>& options)
	{
		MonoObject* managedInstance = metaData.scriptClass->createInstance();
		ScriptGpuProgramImportOptions* scriptObj = ScriptGpuProgramImportOptions::toNative(managedInstance);
		scriptObj->mImportOptions = options;

		return managedInstance;
	}

	void ScriptGpuProgramImportOptions::internal_CreateInstance(MonoObject* instance)
	{
		ScriptGpuProgramImportOptions* nativeInstance = new (bs_alloc<ScriptGpuProgramImportOptions>()) ScriptGpuProgramImportOptions(instance);
	}

	MonoString* ScriptGpuProgramImportOptions::internal_GetEntryPoint(ScriptGpuProgramImportOptions* thisPtr)
	{
		return MonoUtil::stringToMono(MonoManager::instance().getDomain(), thisPtr->getGpuProgImportOptions()->getEntryPoint());
	}

	void ScriptGpuProgramImportOptions::internal_SetEntryPoint(ScriptGpuProgramImportOptions* thisPtr, MonoString* value)
	{
		thisPtr->getGpuProgImportOptions()->setEntryPoint(MonoUtil::monoToString(value));
	}

	GpuLanguage ScriptGpuProgramImportOptions::internal_GetLanguage(ScriptGpuProgramImportOptions* thisPtr)
	{
		return Renderer::getGpuLanguageType(thisPtr->getGpuProgImportOptions()->getLanguage());
	}

	void ScriptGpuProgramImportOptions::internal_SetLanguage(ScriptGpuProgramImportOptions* thisPtr, GpuLanguage value)
	{
		thisPtr->getGpuProgImportOptions()->setLanguage(Renderer::getGpuLanguageName(value));
	}

	GpuProgramProfile ScriptGpuProgramImportOptions::internal_GetProfile(ScriptGpuProgramImportOptions* thisPtr)
	{
		return thisPtr->getGpuProgImportOptions()->getProfile();
	}

	void ScriptGpuProgramImportOptions::internal_SetProfile(ScriptGpuProgramImportOptions* thisPtr, GpuProgramProfile value)
	{
		thisPtr->getGpuProgImportOptions()->setProfile(value);
	}

	GpuProgramType ScriptGpuProgramImportOptions::internal_GetType(ScriptGpuProgramImportOptions* thisPtr)
	{
		return thisPtr->getGpuProgImportOptions()->getType();
	}

	void ScriptGpuProgramImportOptions::internal_SetType(ScriptGpuProgramImportOptions* thisPtr, GpuProgramType value)
	{
		thisPtr->getGpuProgImportOptions()->setType(value);
	}

	ScriptScriptCodeImportOptions::ScriptScriptCodeImportOptions(MonoObject* instance)
		:ScriptObject(instance)
	{
		mImportOptions = bs_shared_ptr<ScriptCodeImportOptions>();
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