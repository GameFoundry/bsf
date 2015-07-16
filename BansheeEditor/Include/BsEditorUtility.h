#pragma once

#include "BsEditorPrerequisites.h"
#include "BsAABox.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains miscelanous helper methods.
	 */
	class BS_ED_EXPORT EditorUtility
	{
	public:
		/**
		 * @brief	Calculates world space bounds of the specified scene object. This will
		 *			consider components with bounds like Renderable.
		 */
		static AABox calculateBounds(const HSceneObject& object);

		/**
		 * @brief	Calculates world space bounds of the specified scene objects. This will
		 *			consider components with bounds like Renderable.
		 */
		static AABox calculateBounds(const Vector<HSceneObject>& objects);

	private:
		/**
		 * @brief	Retrieves all components containing meshes on the specified object and outputs
		 *			their bounds.
		 *
		 * @param	object	Object to calculate bounds for.
		 * @param	bounds	Output bounds, if successful.
		 *
		 * @returns true if a mesh component was found, otherwise false (bounds will not be updated).
		 */
		static bool calculateMeshBounds(const HSceneObject& object, AABox& bounds);
	};
}