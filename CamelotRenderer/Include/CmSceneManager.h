#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

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

		GameObjectPtr getRootNode() const { return mRootNode; }

		/**
		 * @brief	Returns all cameras in the scene.
		 */
		const vector<CameraPtr>::type& getAllCameras() const { return mCachedCameras; }

		vector<RenderablePtr>::type getVisibleRenderables(const CameraPtr camera) const;

	private:
		friend class GameObject;
		GameObjectPtr mRootNode;

		/**
		 * @brief	Register a new node in the scene manager, on the top-most level of the hierarchy.
		 * 			
		 * @note	After you add a node in the scene manager, it takes ownership of its memory and is responsible for releasing it.
		 * 			Do NOT add nodes that have already been added (i.e. if you just want to change their parent). Normally this method will only be called by GameObject.
		 *
		 * @param [in]	node	Node you wish to add. It's your responsibility not to add duplicate or null nodes. This method won't check.
		 */
		void registerNewGO(GameObjectPtr node);

		void notifyComponentAdded(ComponentPtr component);
		void notifyComponentRemoved(ComponentPtr component);

		vector<CameraPtr>::type mCachedCameras;
	};

	CM_EXPORT SceneManager& gSceneManager();
}