//************************************ B3D Framework - Copyright 2025 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "B3DScriptFontFamilyCreateInformation.generated.h"
#include "B3DMonoMethod.h"
#include "B3DMonoClass.h"
#include "B3DMonoUtil.h"
#include "../../../Engine/Core/Text/B3DFontFamily.h"
#include "B3DScriptFontFamilyFace.generated.h"

namespace b3d
{
	ScriptFontFamilyCreateInformation::ScriptFontFamilyCreateInformation()
	{ }

	MonoObject* ScriptFontFamilyCreateInformation::Box(const __FontFamilyCreateInformationInterop& value)
	{
		return MonoUtil::Box(sInteropMetaData.ScriptClass->GetInternalClass(), (void*)&value);
	}

	__FontFamilyCreateInformationInterop ScriptFontFamilyCreateInformation::Unbox(MonoObject* value)
	{
		return *(__FontFamilyCreateInformationInterop*)MonoUtil::Unbox(value);
	}

	FontFamilyCreateInformation ScriptFontFamilyCreateInformation::FromInterop(const __FontFamilyCreateInformationInterop& value)
	{
		FontFamilyCreateInformation output;
		String tmpName;
		tmpName = MonoUtil::MonoToString(value.Name);
		output.Name = tmpName;
		Vector<FontFamilyFace> vecFaces;
		if(value.Faces != nullptr)
		{
			ScriptArray scriptArrayFaces(value.Faces);
			vecFaces.resize(scriptArrayFaces.Size());
			for(int elementIndex = 0; elementIndex < (int)scriptArrayFaces.Size(); elementIndex++)
			{
				vecFaces[elementIndex] = ScriptFontFamilyFace::FromInterop(scriptArrayFaces.Get<__FontFamilyFaceInterop>(elementIndex));
			}
		}
		output.Faces = vecFaces;

		return output;
	}

	__FontFamilyCreateInformationInterop ScriptFontFamilyCreateInformation::ToInterop(const FontFamilyCreateInformation& value)
	{
		__FontFamilyCreateInformationInterop output;
		MonoString* tmpName;
		tmpName = MonoUtil::StringToMono(value.Name);
		output.Name = tmpName;
		int elementCountFaces = (int)value.Faces.size();
		MonoArray* vecFaces;
		ScriptArray scriptArrayFaces = ScriptArray::Create<ScriptFontFamilyFace>(elementCountFaces);
		for(int elementIndex = 0; elementIndex < elementCountFaces; elementIndex++)
		{
			scriptArrayFaces.Set(elementIndex, ScriptFontFamilyFace::ToInterop(value.Faces[elementIndex]));
		}
		vecFaces = scriptArrayFaces.GetInternal();
		output.Faces = vecFaces;

		return output;
	}

}
