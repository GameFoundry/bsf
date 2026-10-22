//************************************ B3D Framework - Copyright 2025 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "B3DScriptFontFamily.generated.h"
#include "B3DMonoMethod.h"
#include "B3DMonoClass.h"
#include "B3DMonoUtil.h"
#include "../../../Engine/Core/Text/B3DFontFamily.h"
#include "B3DScriptResourceManager.h"
#include "Wrappers/B3DScriptRRefBase.h"
#include "../../../Engine/Core/Text/B3DFont.h"
#include "B3DScriptFont.generated.h"
#include "B3DScriptFontFaceStyle.generated.h"

namespace b3d
{
	ScriptFontFamily::ScriptFontFamily(const TResourceHandle<FontFamily>& nativeObject)
		:TScriptResourceWrapper(nativeObject)
	{
		RegisterEvents();
	}

	ScriptFontFamily::~ScriptFontFamily()
	{
		UnregisterEvents();
	}

	void ScriptFontFamily::SetupScriptBindings()
	{
		sInteropMetaData.ScriptClass->AddInternalCall("Internal_GetRef", (void*)&ScriptFontFamily::InternalGetRef);
		sInteropMetaData.ScriptClass->AddInternalCall("Internal_GetFamilyName", (void*)&ScriptFontFamily::InternalGetFamilyName);
		sInteropMetaData.ScriptClass->AddInternalCall("Internal_GetFace", (void*)&ScriptFontFamily::InternalGetFace);
		sInteropMetaData.ScriptClass->AddInternalCall("Internal_HasFace", (void*)&ScriptFontFamily::InternalHasFace);
		sInteropMetaData.ScriptClass->AddInternalCall("Internal_AddFace", (void*)&ScriptFontFamily::InternalAddFace);
		sInteropMetaData.ScriptClass->AddInternalCall("Internal_RemoveFace", (void*)&ScriptFontFamily::InternalRemoveFace);

	}

	MonoObject* ScriptFontFamily::CreateScriptObject(bool construct)
	{
		bool dummy = false;
		void* ctorParams[1] = { &dummy };

		if(construct)
			return sInteropMetaData.ScriptClass->CreateInstance("bool", ctorParams);

		return sInteropMetaData.ScriptClass->CreateInstance(false);
	}
	MonoObject* ScriptFontFamily::InternalGetRef(ScriptFontFamily* self)
	{
		return self->GetOrCreateResourceReference();
	}

	MonoString* ScriptFontFamily::InternalGetFamilyName(ScriptFontFamily* self)
	{
		String tmp__output;
		if(!self->IsNativeObjectValid())
			return {};

		tmp__output = static_cast<FontFamily*>(self->GetNativeObject())->GetFamilyName();

		MonoString* __output;
		__output = MonoUtil::StringToMono(tmp__output);

		return __output;
	}

	MonoObject* ScriptFontFamily::InternalGetFace(ScriptFontFamily* self, FontFaceStyle* style)
	{
		TResourceHandle<Font> tmp__output;
		if(!self->IsNativeObjectValid())
			return {};

		tmp__output = static_cast<FontFamily*>(self->GetNativeObject())->GetFace(*style);

		MonoObject* __output;
		MonoObject* temp__output = nullptr;
		if(tmp__output)
		temp__output = ScriptResourceWrapper::GetOrCreateScriptObject(tmp__output);
		__output = temp__output;

		return __output;
	}

	bool ScriptFontFamily::InternalHasFace(ScriptFontFamily* self, FontFaceStyle* style)
	{
		bool tmp__output;
		if(!self->IsNativeObjectValid())
			return {};

		tmp__output = static_cast<FontFamily*>(self->GetNativeObject())->HasFace(*style);

		bool __output;
		__output = tmp__output;

		return __output;
	}

	bool ScriptFontFamily::InternalAddFace(ScriptFontFamily* self, MonoObject* font, FontFaceStyle* style)
	{
		bool tmp__output;
		if(!self->IsNativeObjectValid())
			return {};

		TResourceHandle<Font> tmpfont;
		ScriptRRefBase* scriptObjectWrapperfont;
		scriptObjectWrapperfont = ScriptRRefBase::GetScriptObjectWrapper(font);
		if(scriptObjectWrapperfont != nullptr)
			tmpfont = B3DStaticResourceCast<Font>(scriptObjectWrapperfont->GetNativeObject());
		tmp__output = static_cast<FontFamily*>(self->GetNativeObject())->AddFace(tmpfont, *style);

		bool __output;
		__output = tmp__output;

		return __output;
	}

	bool ScriptFontFamily::InternalRemoveFace(ScriptFontFamily* self, FontFaceStyle* style)
	{
		bool tmp__output;
		if(!self->IsNativeObjectValid())
			return {};

		tmp__output = static_cast<FontFamily*>(self->GetNativeObject())->RemoveFace(*style);

		bool __output;
		__output = tmp__output;

		return __output;
	}
}
