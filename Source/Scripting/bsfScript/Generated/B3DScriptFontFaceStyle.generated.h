//************************************ B3D Framework - Copyright 2025 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "B3DScriptEnginePrerequisites.h"
#include "B3DScriptObjectWrapper.h"
#include "../../../Engine/Core/Text/B3DFontFace.h"

namespace b3d
{
	class B3D_SCRIPT_INTEROP_EXPORT ScriptFontFaceStyle : public TScriptTypeDefinition<ScriptFontFaceStyle>
	{
	public:
		B3D_SCRIPT_TYPE_DEFINITION(kEngineAssembly, kEngineNs, "FontFaceStyle")

		static MonoObject* Box(const FontFaceStyle& value);
		static FontFaceStyle Unbox(MonoObject* value);

	private:
		ScriptFontFaceStyle();

	};
}
