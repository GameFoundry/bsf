//************************************ B3D Framework - Copyright 2025 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "B3DScriptFontFamilyFace.generated.h"
#include "B3DMonoMethod.h"
#include "B3DMonoClass.h"
#include "B3DMonoUtil.h"
#include "B3DScriptResourceManager.h"
#include "Wrappers/B3DScriptRRefBase.h"
#include "../../../Engine/Core/Text/B3DFont.h"
#include "B3DScriptFont.generated.h"
#include "../../../Engine/Core/Text/B3DFontFace.h"
#include "B3DScriptFontFaceStyle.generated.h"

namespace b3d
{
	ScriptFontFamilyFace::ScriptFontFamilyFace()
	{ }

	MonoObject* ScriptFontFamilyFace::Box(const __FontFamilyFaceInterop& value)
	{
		return MonoUtil::Box(sInteropMetaData.ScriptClass->GetInternalClass(), (void*)&value);
	}

	__FontFamilyFaceInterop ScriptFontFamilyFace::Unbox(MonoObject* value)
	{
		return *(__FontFamilyFaceInterop*)MonoUtil::Unbox(value);
	}

	FontFamilyFace ScriptFontFamilyFace::FromInterop(const __FontFamilyFaceInterop& value)
	{
		FontFamilyFace output;
		TResourceHandle<Font> tmpFont;
		ScriptRRefBase* scriptObjectWrapperFont;
		scriptObjectWrapperFont = ScriptRRefBase::GetScriptObjectWrapper(value.Font);
		if(scriptObjectWrapperFont != nullptr)
			tmpFont = B3DStaticResourceCast<Font>(scriptObjectWrapperFont->GetNativeObject());
		output.Font = tmpFont;
		output.Style = value.Style;

		return output;
	}

	__FontFamilyFaceInterop ScriptFontFamilyFace::ToInterop(const FontFamilyFace& value)
	{
		__FontFamilyFaceInterop output;
		MonoObject* tmpFont;
		ScriptRRefBase* scriptWrapperObjectFont;
		scriptWrapperObjectFont = ScriptResourceManager::Instance().GetScriptRRef(value.Font);
		if(scriptWrapperObjectFont != nullptr)
			tmpFont = scriptWrapperObjectFont->GetScriptObject();
		else
			tmpFont = nullptr;
		output.Font = tmpFont;
		output.Style = value.Style;

		return output;
	}

}
