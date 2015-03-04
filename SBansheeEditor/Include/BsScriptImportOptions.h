#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsPixelData.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptImportOptionsBase : ScriptObjectBase
	{
	public:
		SPtr<ImportOptions> getImportOptions() const { return mImportOptions; }

	protected:
		ScriptImportOptionsBase(MonoObject* instance);
		virtual ~ScriptImportOptionsBase() {}

		SPtr<ImportOptions> mImportOptions;
	};

	class BS_SCR_BED_EXPORT ScriptImportOptions : public ScriptObject <ScriptImportOptions, ScriptImportOptionsBase>
	{
	public:
		static String getAssemblyName() { return BansheeEditorAssemblyName; }
		static String getNamespace() { return "BansheeEditor"; }
		static String getTypeName() { return "ImportOptions"; }
		static void initRuntimeData() { }

		static MonoObject* create(const SPtr<ImportOptions>& importOptions);

	private:
		ScriptImportOptions(MonoObject* instance)
			:ScriptObject(instance)
		{ }
	};

	class BS_SCR_BED_EXPORT ScriptTextureImportOptions : public ScriptObject<ScriptTextureImportOptions, ScriptImportOptionsBase>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "TextureImportOptions")

		static MonoObject* create();
		static MonoObject* create(const SPtr<TextureImportOptions>& options);

	private:
		SPtr<TextureImportOptions> getTexImportOptions();

		static void internal_CreateInstance(MonoObject* instance);
		static PixelFormat internal_GetPixelFormat(ScriptTextureImportOptions* thisPtr);
		static void internal_SetPixelFormat(ScriptTextureImportOptions* thisPtr, PixelFormat value);
		static bool internal_GetGenerateMipmaps(ScriptTextureImportOptions* thisPtr);
		static void internal_SetGenerateMipmaps(ScriptTextureImportOptions* thisPtr, bool value);
		static UINT32 internal_GetMaxMipmapLevel(ScriptTextureImportOptions* thisPtr);
		static void internal_SetMaxMipmapLevel(ScriptTextureImportOptions* thisPtr, UINT32 value);

		ScriptTextureImportOptions(MonoObject* instance);
	};

	class BS_SCR_BED_EXPORT ScriptFontImportOptions : public ScriptObject <ScriptFontImportOptions, ScriptImportOptionsBase>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "FontImportOptions")

		static MonoObject* create();
		static MonoObject* create(const SPtr<FontImportOptions>& options);

	private:
		SPtr<FontImportOptions> getFontImportOptions();

		static void internal_CreateInstance(MonoObject* instance);
		static MonoArray* internal_GetFontSizes(ScriptFontImportOptions* thisPtr);
		static void internal_SetFontSizes(ScriptFontImportOptions* thisPtr, MonoArray* value);
		static UINT32 internal_GetDPI(ScriptFontImportOptions* thisPtr);
		static void internal_SetDPI(ScriptFontImportOptions* thisPtr, UINT32 value);
		static bool internal_GetAntialiasing(ScriptFontImportOptions* thisPtr);
		static void internal_SetAntialiasing(ScriptFontImportOptions* thisPtr, bool value);
		static MonoArray* internal_GetCharRanges(ScriptFontImportOptions* thisPtr);
		static void internal_SetCharRanges(ScriptFontImportOptions* thisPtr, MonoArray* value);

		ScriptFontImportOptions(MonoObject* instance);
	};

	class BS_SCR_BED_EXPORT ScriptGpuProgramImportOptions : public ScriptObject <ScriptGpuProgramImportOptions, ScriptImportOptionsBase>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "GpuProgramImportOptions")

		static MonoObject* create();
		static MonoObject* create(const SPtr<GpuProgramImportOptions>& options);

	private:
		SPtr<GpuProgramImportOptions> getGpuProgImportOptions();

		static void internal_CreateInstance(MonoObject* instance);
		static MonoString* internal_GetEntryPoint(ScriptGpuProgramImportOptions* thisPtr);
		static void internal_SetEntryPoint(ScriptGpuProgramImportOptions* thisPtr, MonoString* value);
		static GpuLanguage internal_GetLanguage(ScriptGpuProgramImportOptions* thisPtr);
		static void internal_SetLanguage(ScriptGpuProgramImportOptions* thisPtr, GpuLanguage value);
		static GpuProgramProfile internal_GetProfile(ScriptGpuProgramImportOptions* thisPtr);
		static void internal_SetProfile(ScriptGpuProgramImportOptions* thisPtr, GpuProgramProfile value);
		static GpuProgramType internal_GetType(ScriptGpuProgramImportOptions* thisPtr);
		static void internal_SetType(ScriptGpuProgramImportOptions* thisPtr, GpuProgramType value);

		ScriptGpuProgramImportOptions(MonoObject* instance);
	};

	class BS_SCR_BED_EXPORT ScriptCharRange : public ScriptObject <ScriptCharRange>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "Color")

		static Color unbox(MonoObject* obj);
		static MonoObject* box(const Color& value);

	private:
		ScriptCharRange(MonoObject* instance);
	};
}