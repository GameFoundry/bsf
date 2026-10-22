//************************************ B3D Framework - Copyright 2025 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "B3DScriptEnginePrerequisites.h"
#include "B3DScriptResourceWrapper.h"
#include "../../../Engine/Core/Text/B3DFontFace.h"

namespace b3d { class FontFamily; }
namespace b3d
{
	class B3D_SCRIPT_INTEROP_EXPORT ScriptFontFamily : public TScriptResourceWrapper<FontFamily, ScriptFontFamily>
	{
	public:
		B3D_SCRIPT_TYPE_DEFINITION(kEngineAssembly, kEngineNs, "FontFamily")

		ScriptFontFamily(const TResourceHandle<FontFamily>& nativeObject);
		~ScriptFontFamily();

		static void SetupScriptBindings();

		static MonoObject* CreateScriptObject(bool construct);

	private:
		static MonoObject* InternalGetRef(ScriptFontFamily* self);

		static MonoString* InternalGetFamilyName(ScriptFontFamily* self);
		static MonoObject* InternalGetFace(ScriptFontFamily* self, FontFaceStyle* style);
		static bool InternalHasFace(ScriptFontFamily* self, FontFaceStyle* style);
		static bool InternalAddFace(ScriptFontFamily* self, MonoObject* font, FontFaceStyle* style);
		static bool InternalRemoveFace(ScriptFontFamily* self, FontFaceStyle* style);
	};
}
