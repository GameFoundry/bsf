//************************************ B3D Framework - Copyright 2025 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "B3DScriptEnginePrerequisites.h"
#include "B3DScriptObjectWrapper.h"
#include "../../../Engine/Core/Text/B3DFontFamily.h"
#include "../../../Engine/Core/Text/B3DFontFamily.h"
#include "B3DScriptFontFamilyFace.generated.h"

namespace b3d
{
	struct __FontFamilyCreateInformationInterop
	{
		MonoString* Name;
		MonoArray* Faces;
	};

	class B3D_SCRIPT_INTEROP_EXPORT ScriptFontFamilyCreateInformation : public TScriptTypeDefinition<ScriptFontFamilyCreateInformation>
	{
	public:
		B3D_SCRIPT_TYPE_DEFINITION(kEngineAssembly, kEngineNs, "FontFamilyCreateInformation")

		static MonoObject* Box(const __FontFamilyCreateInformationInterop& value);
		static __FontFamilyCreateInformationInterop Unbox(MonoObject* value);
		static FontFamilyCreateInformation FromInterop(const __FontFamilyCreateInformationInterop& value);
		static __FontFamilyCreateInformationInterop ToInterop(const FontFamilyCreateInformation& value);

	private:
		ScriptFontFamilyCreateInformation();

	};
}
