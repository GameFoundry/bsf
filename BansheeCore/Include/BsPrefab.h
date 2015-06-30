#pragma once

#include "BsCorePrerequisites.h"
#include "BsGameObject.h"
#include "BsResource.h"

namespace BansheeEngine
{
	/**
	 * @brief	Prefab is a saveable hierarchy of scene objects. In general it
	 *			can serve as any grouping of scene objects (e.g. a level) or be used
	 *			as a form of a template instantiated and reused throughout the scene.
	 */
	class BS_CORE_EXPORT Prefab : public Resource
	{
	public:
		Prefab();

		/**
		 * @brief	Creates a new prefab from the provided scene object. The scene object
		 *			must not have a prefab link already. After the prefab is created the
		 *			scene object will be automatically linked to it.
		 */
		static HPrefab create(const HSceneObject& sceneObject);

		/**
		 * @brief	Instantiates a prefab by creating an instance of the prefab's
		 *			scene object hierarchy. The returned hierarchy will be parented
		 *			to world root by default.
		 */
		HSceneObject instantiate();

		/**
		 * @brief	Returns a reference to the internal prefab hierarchy.
		 */
		HSceneObject getRoot() const { return mRoot; }

	private:
		/**
		 * @brief	Initializes the internal prefab hierarchy. Must be called druing creation.
		 */
		void initialize(const HSceneObject& sceneObject);

		/**
		 * @brief	Creates an empty and uninitialized prefab.
		 */
		static PrefabPtr createEmpty();

		HSceneObject mRoot;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class PrefabRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}