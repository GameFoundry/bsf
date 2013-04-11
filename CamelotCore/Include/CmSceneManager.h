#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmGameObject.h"

namespace CamelotEngine
{
	/**
	 * @brief	Manages all objects in the scene and provides various query methods
	 * 			for finding objects you need. This is just the base class with basic
	 * 			query functionality. You should override it with your own version that
	 * 			implements a spatial data structure of your choice for faster queries.
	 */
	class CM_EXPORT SceneManager : public Module<SceneManager>
	{
	public:
		SceneManager();
		virtual ~SceneManager();

		HSceneObject getRootNode() const { return mRootNode; }

		virtual void update();

		/**
		 * @brief	Returns all cameras in the scene.
		 */
		const vector<HCamera>::type& getAllCameras() const { return mCachedCameras; }

		vector<HRenderable>::type getVisibleRenderables(const HCamera& camera) const;

	private:
		friend class SceneObject;
		HSceneObject mRootNode;

		/**
		 * @brief	Register a new node in the scene manager, on the top-most level of the hierarchy.
		 * 			
		 * @note	After you add a node in the scene manager, it takes ownership of its memory and is responsible for releasing it.
		 * 			Do NOT add nodes that have already been added (i.e. if you just want to change their parent). Normally this method will only be called by SceneObject.
		 *
		 * @param [in]	node	Node you wish to add. It's your responsibility not to add duplicate or null nodes. This method won't check.
		 */
		void registerNewGO(const HSceneObject& node);

		void notifyComponentAdded(const HComponent& component);
		void notifyComponentRemoved(const HComponent& component);

		vector<HCamera>::type mCachedCameras;
	};

	CM_EXPORT SceneManager& gSceneManager();
}