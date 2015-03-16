#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsPlatformInfo.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptPlatformInfoBase : public ScriptObjectBase
	{
	public:
		SPtr<PlatformInfo> getPlatformInfo() const { return mPlatformInfo; }

	protected:
		ScriptPlatformInfoBase(MonoObject* instance);
		virtual ~ScriptPlatformInfoBase() {}

		SPtr<PlatformInfo> mPlatformInfo;
	};

	class BS_SCR_BED_EXPORT ScriptPlatformInfo : public ScriptObject <ScriptPlatformInfo, ScriptPlatformInfoBase>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "PlatformInfo")

		static MonoObject* create(const SPtr<PlatformInfo>& platformInfo);
	private:
		ScriptPlatformInfo(MonoObject* instance);

		static PlatformType internal_GetType(ScriptPlatformInfoBase* thisPtr);
		static MonoString* internal_GetDefines(ScriptPlatformInfoBase* thisPtr);
		static void internal_SetDefines(ScriptPlatformInfoBase* thisPtr, MonoString* value);
	};

	class BS_SCR_BED_EXPORT ScriptWinPlatformInfo : public ScriptObject <ScriptWinPlatformInfo, ScriptPlatformInfoBase>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "WinPlatformInfo")

		static MonoObject* create(const SPtr<WinPlatformInfo>& platformInfo);

	private:
		ScriptWinPlatformInfo(MonoObject* instance);
		SPtr<WinPlatformInfo> getWinPlatformInfo() const;

		static bool internal_GetIs32Bit(ScriptWinPlatformInfo* thisPtr);
		static void internal_SetIs32Bit(ScriptWinPlatformInfo* thisPtr, bool value);
	};
}