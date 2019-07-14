//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Scene/BsGameObject.h"

namespace bs
{
	/** @addtogroup Scene
	 *  @{
	 */

	/** Performs various prefab specific operations. */
	class BS_CORE_EXPORT PrefabUtility
	{
	public:
		/**
		 * Remove any instance specific changes to the object or its hierarchy from the provided prefab instance and
		 * restore it to the exact copy of the linked prefab.
		 *
		 * @param[in]	so	Object to revert.
		 */
		static void revertToPrefab(const HSceneObject& so);

		/**
		 * Updates all of the objects belonging to the same prefab instance as the provided object (if any). The update
		 * will apply any changes from the linked prefab to the hierarchy (if any).
		 *
		 * @param[in]	so	Object to update.
		 */
		static void updateFromPrefab(const HSceneObject& so);

		/**
		 * Generates prefab "link" ID that can be used for tracking which game object in a prefab instance corresponds to
		 * an object in the prefab.
		 *
		 * @note	If any children of the provided object belong to another prefab they will not have IDs generated.
		 */
		static void generatePrefabIds(const HSceneObject& sceneObject);

		/**
		 * Clears all prefab "link" IDs in the provided object and its children.
		 *
		 * @param[in]	sceneObject		Prefab instance to clear the link IDs from.
		 * @param[in]	recursive		If true, all children of the provided scene object will be cleared as well.
		 * @param[in]	clearRoot		If true, the root provided object will have its link ID cleared. If false the root
		 *								object's components will have their IDs cleared but not the scene object itself.
		 *								(Child scene objects will have their link IDs cleared in case @p recursive is true.)
		 *
		 * @note	If any of its children belong to another prefab they will not be cleared.
		 */
		static void clearPrefabIds(const HSceneObject& sceneObject, bool recursive = true, bool clearRoot = true);

		/**
		 * Updates the internal prefab diff data by recording the difference between the current values in the provided
		 * prefab instance and its prefab.
		 *
		 * @note	
		 * If the provided object contains any child prefab instances, this will be done recursively for them as well.
		 */
		static void recordPrefabDiff(const HSceneObject& sceneObject);
	};

	/** @} */
}
