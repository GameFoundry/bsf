#pragma once

#include "BsEditorPrerequisites.h"
#include "BsAABox.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT EditorUtility
	{
	public:
		static AABox calculateBounds(const HSceneObject& object);
		static AABox calculateBounds(const Vector<HSceneObject>& objects);

	private:
		static bool calculateMeshBounds(const HSceneObject& object, AABox& bounds);
	};
}