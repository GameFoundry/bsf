#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsPlatformInfo.h"

namespace BansheeEngine
{
	/**
	 * @brief	Base class for all C++/CLR interop objects dealing with specific
	 *			PlatformInfo implementations.
	 */
	class BS_SCR_BED_EXPORT ScriptPlatformInfoBase : public ScriptObjectBase
	{
	public:
		/**
		 * @brief	Returns the internal native platform info object.
		 */
		SPtr<PlatformInfo> getPlatformInfo() const { return mPlatformInfo; }

	protected:
		ScriptPlatformInfoBase(MonoObject* instance);
		virtual ~ScriptPlatformInfoBase() {}

		SPtr<PlatformInfo> mPlatformInfo;
	};

	/**
	 * @brief	Interop class between C++ & CLR for PlatformInfo.
	 */
	class BS_SCR_BED_EXPORT ScriptPlatformInfo : public ScriptObject <ScriptPlatformInfo, ScriptPlatformInfoBase>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "PlatformInfo")

		/**
		 * @brief	Creates a new managed platform info object that wraps the provided
		 *			native platform info.
		 */
		static MonoObject* create(const SPtr<PlatformInfo>& platformInfo);
	private:
		ScriptPlatformInfo(MonoObject* instance);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static PlatformType internal_GetType(ScriptPlatformInfoBase* thisPtr);
		static MonoString* internal_GetDefines(ScriptPlatformInfoBase* thisPtr);
		static void internal_SetDefines(ScriptPlatformInfoBase* thisPtr, MonoString* value);
	};

	/**
	 * @brief	Interop class between C++ & CLR for WinPlatformInfo.
	 */
	class BS_SCR_BED_EXPORT ScriptWinPlatformInfo : public ScriptObject <ScriptWinPlatformInfo, ScriptPlatformInfoBase>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "WinPlatformInfo")

		/**
		 * @brief	Creates a new managed platform info object that wraps the provided
		 *			native platform info.
		 */
		static MonoObject* create(const SPtr<WinPlatformInfo>& platformInfo);

	private:
		ScriptWinPlatformInfo(MonoObject* instance);

		/**
		 * @brief	Returns the internal native Windows platform info object.
		 */
		SPtr<WinPlatformInfo> getWinPlatformInfo() const;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static bool internal_GetIs32Bit(ScriptWinPlatformInfo* thisPtr);
		static void internal_SetIs32Bit(ScriptWinPlatformInfo* thisPtr, bool value);
		static MonoObject* internal_GetIcon(ScriptWinPlatformInfo* thisPtr, int size);
		static void Internal_SetIcon(ScriptWinPlatformInfo* thisPtr, int size, ScriptTexture2D* texturePtr);
	};
}