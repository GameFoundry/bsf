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
#include "BsRenderer.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	struct CharRange
	{
		UINT32 start, end;
	};

	ScriptCharRange::ScriptCharRange(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptCharRange::initRuntimeData()
	{ }

	ScriptTextureImportOptions::ScriptTextureImportOptions(MonoObject* instance)
		:ScriptObject(instance), mImportOptions(bs_shared_ptr<TextureImportOptions>())
	{

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

	MonoObject* ScriptTextureImportOptions::create()
	{
		return metaData.scriptClass->createInstance();
	}

	void ScriptTextureImportOptions::internal_CreateInstance(MonoObject* instance)
	{
		ScriptTextureImportOptions* nativeInstance = new (bs_alloc<ScriptTextureImportOptions>()) ScriptTextureImportOptions(instance);
	}

	PixelFormat ScriptTextureImportOptions::internal_GetPixelFormat(ScriptTextureImportOptions* thisPtr)
	{
		return thisPtr->mImportOptions->getFormat();
	}

	void ScriptTextureImportOptions::internal_SetPixelFormat(ScriptTextureImportOptions* thisPtr, PixelFormat value)
	{
		thisPtr->mImportOptions->setFormat(value);
	}

	bool ScriptTextureImportOptions::internal_GetGenerateMipmaps(ScriptTextureImportOptions* thisPtr)
	{
		return thisPtr->mImportOptions->getGenerateMipmaps();
	}

	void ScriptTextureImportOptions::internal_SetGenerateMipmaps(ScriptTextureImportOptions* thisPtr, bool value)
	{
		thisPtr->mImportOptions->setGenerateMipmaps(value);
	}

	UINT32 ScriptTextureImportOptions::internal_GetMaxMipmapLevel(ScriptTextureImportOptions* thisPtr)
	{
		return thisPtr->mImportOptions->getMaxMip();
	}

	void ScriptTextureImportOptions::internal_SetMaxMipmapLevel(ScriptTextureImportOptions* thisPtr, UINT32 value)
	{
		thisPtr->mImportOptions->setMaxMip(value);
	}

	ScriptFontImportOptions::ScriptFontImportOptions(MonoObject* instance)
		:ScriptObject(instance), mImportOptions(bs_shared_ptr<FontImportOptions>())
	{

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

	MonoObject* ScriptFontImportOptions::create()
	{
		return metaData.scriptClass->createInstance();
	}

	void ScriptFontImportOptions::internal_CreateInstance(MonoObject* instance)
	{
		ScriptFontImportOptions* nativeInstance = new (bs_alloc<ScriptFontImportOptions>()) ScriptFontImportOptions(instance);
	}

	MonoArray* ScriptFontImportOptions::internal_GetFontSizes(ScriptFontImportOptions* thisPtr)
	{
		Vector<UINT32> fontSizes = thisPtr->mImportOptions->getFontSizes();

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

		thisPtr->mImportOptions->setFontSizes(fontSizes);
	}

	UINT32 ScriptFontImportOptions::internal_GetDPI(ScriptFontImportOptions* thisPtr)
	{
		return thisPtr->mImportOptions->getDPI();
	}

	void ScriptFontImportOptions::internal_SetDPI(ScriptFontImportOptions* thisPtr, UINT32 value)
	{
		thisPtr->mImportOptions->setDPI(value);
	}

	bool ScriptFontImportOptions::internal_GetAntialiasing(ScriptFontImportOptions* thisPtr)
	{
		return thisPtr->mImportOptions->getAntialiasing();
	}

	void ScriptFontImportOptions::internal_SetAntialiasing(ScriptFontImportOptions* thisPtr, bool value)
	{
		thisPtr->mImportOptions->setAntialiasing(value);
	}

	MonoArray* ScriptFontImportOptions::internal_GetCharRanges(ScriptFontImportOptions* thisPtr)
	{
		Vector<std::pair<UINT32, UINT32>> charRanges = thisPtr->mImportOptions->getCharIndexRanges();

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

		thisPtr->mImportOptions->clearCharIndexRanges();
		for (UINT32 i = 0; i < inArray.size(); i++)
		{
			CharRange range = inArray.get<CharRange>(i);
			thisPtr->mImportOptions->addCharIndexRange(range.start, range.end);
		}
	}

	ScriptGpuProgramImportOptions::ScriptGpuProgramImportOptions(MonoObject* instance)
		:ScriptObject(instance), mImportOptions(bs_shared_ptr<GpuProgramImportOptions>())
	{

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

	MonoObject* ScriptGpuProgramImportOptions::create()
	{
		return metaData.scriptClass->createInstance();
	}

	void ScriptGpuProgramImportOptions::internal_CreateInstance(MonoObject* instance)
	{
		ScriptGpuProgramImportOptions* nativeInstance = new (bs_alloc<ScriptGpuProgramImportOptions>()) ScriptGpuProgramImportOptions(instance);
	}

	MonoString* ScriptGpuProgramImportOptions::internal_GetEntryPoint(ScriptGpuProgramImportOptions* thisPtr)
	{
		return MonoUtil::stringToMono(MonoManager::instance().getDomain(), thisPtr->mImportOptions->getEntryPoint());
	}

	void ScriptGpuProgramImportOptions::internal_SetEntryPoint(ScriptGpuProgramImportOptions* thisPtr, MonoString* value)
	{
		thisPtr->mImportOptions->setEntryPoint(MonoUtil::monoToString(value));
	}

	GpuLanguage ScriptGpuProgramImportOptions::internal_GetLanguage(ScriptGpuProgramImportOptions* thisPtr)
	{
		return Renderer::getGpuLanguageType(thisPtr->mImportOptions->getLanguage());
	}

	void ScriptGpuProgramImportOptions::internal_SetLanguage(ScriptGpuProgramImportOptions* thisPtr, GpuLanguage value)
	{
		thisPtr->mImportOptions->setLanguage(Renderer::getGpuLanguageName(value));
	}

	GpuProgramProfile ScriptGpuProgramImportOptions::internal_GetProfile(ScriptGpuProgramImportOptions* thisPtr)
	{
		return thisPtr->mImportOptions->getProfile();
	}

	void ScriptGpuProgramImportOptions::internal_SetProfile(ScriptGpuProgramImportOptions* thisPtr, GpuProgramProfile value)
	{
		thisPtr->mImportOptions->setProfile(value);
	}

	GpuProgramType ScriptGpuProgramImportOptions::internal_GetType(ScriptGpuProgramImportOptions* thisPtr)
	{
		return thisPtr->mImportOptions->getType();
	}

	void ScriptGpuProgramImportOptions::internal_SetType(ScriptGpuProgramImportOptions* thisPtr, GpuProgramType value)
	{
		thisPtr->mImportOptions->setType(value);
	}
}