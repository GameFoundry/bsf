//************************************ B3D Framework - Copyright 2025 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "B3DScriptFontMetaData.generated.h"
#include "B3DMonoMethod.h"
#include "B3DMonoClass.h"
#include "B3DMonoUtil.h"
#include "B3DScriptFontFaceStyle.generated.h"

namespace b3d
{
	ScriptFontMetaData::ScriptFontMetaData(const TShared<FontMetaData>& nativeObject)
		:TScriptReflectableWrapper(nativeObject)
	{
		RegisterEvents();
	}

	ScriptFontMetaData::~ScriptFontMetaData()
	{
		UnregisterEvents();
	}

	void ScriptFontMetaData::SetupScriptBindings()
	{
		sInteropMetaData.ScriptClass->AddInternalCall("Internal_GetFamilyName", (void*)&ScriptFontMetaData::InternalGetFamilyName);
		sInteropMetaData.ScriptClass->AddInternalCall("Internal_SetFamilyName", (void*)&ScriptFontMetaData::InternalSetFamilyName);
		sInteropMetaData.ScriptClass->AddInternalCall("Internal_GetStyle", (void*)&ScriptFontMetaData::InternalGetStyle);
		sInteropMetaData.ScriptClass->AddInternalCall("Internal_SetStyle", (void*)&ScriptFontMetaData::InternalSetStyle);

	}

	MonoObject* ScriptFontMetaData::CreateScriptObject(bool construct)
	{
		bool dummy = false;
		void* ctorParams[1] = { &dummy };

		if(construct)
			return sInteropMetaData.ScriptClass->CreateInstance("bool", ctorParams);

		return sInteropMetaData.ScriptClass->CreateInstance(false);
	}
	MonoString* ScriptFontMetaData::InternalGetFamilyName(ScriptFontMetaData* self)
	{
		String tmp__output;
		if(!self->IsNativeObjectValid())
			return {};

		tmp__output = static_cast<FontMetaData*>(self->GetNativeObject())->FamilyName;

		MonoString* __output;
		__output = MonoUtil::StringToMono(tmp__output);

		return __output;
	}

	void ScriptFontMetaData::InternalSetFamilyName(ScriptFontMetaData* self, MonoString* value)
	{
		if(!self->IsNativeObjectValid())
			return;

		String tmpvalue;
		tmpvalue = MonoUtil::MonoToString(value);
		static_cast<FontMetaData*>(self->GetNativeObject())->FamilyName = tmpvalue;
	}

	void ScriptFontMetaData::InternalGetStyle(ScriptFontMetaData* self, FontFaceStyle* __output)
	{
		FontFaceStyle tmp__output;
		if(!self->IsNativeObjectValid())
		{
			*__output = {};
			return;
		}

		tmp__output = static_cast<FontMetaData*>(self->GetNativeObject())->Style;

		*__output = tmp__output;


	}

	void ScriptFontMetaData::InternalSetStyle(ScriptFontMetaData* self, FontFaceStyle* value)
	{
		if(!self->IsNativeObjectValid())
			return;

		static_cast<FontMetaData*>(self->GetNativeObject())->Style = *value;
	}
}
