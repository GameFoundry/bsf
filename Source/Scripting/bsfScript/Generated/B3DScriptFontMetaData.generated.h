//************************************ B3D Framework - Copyright 2025 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "B3DScriptEnginePrerequisites.h"
#include "B3DScriptReflectableWrapper.h"
#include "B3DScriptResourceMetaData.generated.h"
#include "../../../Engine/Core/Text/B3DFont.h"
#include "../../../Engine/Core/Text/B3DFontFace.h"

namespace b3d { class FontMetaData; }
namespace b3d
{
	class B3D_SCRIPT_INTEROP_EXPORT ScriptFontMetaData : public TScriptReflectableWrapper<FontMetaData, ScriptFontMetaData, ScriptResourceMetaDataWrapperBase>
	{
	public:
		B3D_SCRIPT_TYPE_DEFINITION(kEngineAssembly, kEngineNs, "FontMetaData")

		ScriptFontMetaData(const TShared<FontMetaData>& nativeObject);
		~ScriptFontMetaData();

		static void SetupScriptBindings();

		static MonoObject* CreateScriptObject(bool construct);

	private:
		static MonoString* InternalGetFamilyName(ScriptFontMetaData* self);
		static void InternalSetFamilyName(ScriptFontMetaData* self, MonoString* value);
		static void InternalGetStyle(ScriptFontMetaData* self, FontFaceStyle* __output);
		static void InternalSetStyle(ScriptFontMetaData* self, FontFaceStyle* value);
	};
}
