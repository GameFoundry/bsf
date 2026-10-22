//************************************ B3D Framework - Copyright 2025 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "B3DScriptEnginePrerequisites.h"
#include "B3DScriptObjectWrapper.h"
#include "../../../Engine/Core/Text/B3DFontFamily.h"
#include "../../../Engine/Core/Text/B3DFontFace.h"

namespace b3d
{
	struct __FontFamilyFaceInterop
	{
		MonoObject* Font;
		FontFaceStyle Style;
	};

	class B3D_SCRIPT_INTEROP_EXPORT ScriptFontFamilyFace : public TScriptTypeDefinition<ScriptFontFamilyFace>
	{
	public:
		B3D_SCRIPT_TYPE_DEFINITION(kEngineAssembly, kEngineNs, "FontFamilyFace")

		static MonoObject* Box(const __FontFamilyFaceInterop& value);
		static __FontFamilyFaceInterop Unbox(MonoObject* value);
		static FontFamilyFace FromInterop(const __FontFamilyFaceInterop& value);
		static __FontFamilyFaceInterop ToInterop(const FontFamilyFace& value);

	private:
		ScriptFontFamilyFace();

	};
}
