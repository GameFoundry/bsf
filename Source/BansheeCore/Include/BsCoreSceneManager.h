//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsGameObject.h"

namespace BansheeEngine
{
	/** @cond INTERNAL */
	/** @addtogroup Scene
	 *  @{
	 */

	/**
	 * Manages all objects in the scene and provides various query methods for finding objects. This is just the base class
	 * with basic query functionality. You should override it with your own version.
	 */
	class BS_CORE_EXPORT CoreSceneManager : public Module<CoreSceneManager>
	{
	public:
		CoreSceneManager();
		virtual ~CoreSceneManager();

		/**	Returns the root scene object. */
		HSceneObject getRootNode() const { return mRootNode; }

		/**
		 * Destroys all scene objects in the scene.
		 *
		 * @param[in]	forceAll	If true, then even the persistent objects will be unloaded.
		 */
		void clearScene(bool forceAll = false);

		/** Changes the root scene object. Any persistent objects will remain in the scene, now parented to the new root. */
		void _setRootNode(const HSceneObject& root);

		/** Called every frame. Calls update methods on all scene objects and their components. */
		virtual void _update();

		/** Updates dirty transforms on any core objects that may be tied with scene objects. */
		virtual void _updateCoreObjectTransforms() { }

	protected:
		friend class SceneObject;

		/**
		 * Register a new node in the scene manager, on the top-most level of the hierarchy.
		 * 			
		 * @param[in]	node	Node you wish to add. It's your responsibility not to add duplicate or null nodes. This 
		 *						method won't check.
		 *
		 * @note	
		 * After you add a node in the scene manager, it takes ownership of its memory and is responsible for releasing it.
		 * Do NOT add nodes that have already been added (i.e. if you just want to change their parent). Normally this 
		 * method will only be called by SceneObject.
		 */
		void registerNewSO(const HSceneObject& node);

	protected:
		HSceneObject mRootNode;
	};

	/**
	 * Handles creation of a scene manager.
	 *
	 * @note	
	 * Since scene manager implementations may vary it is expected that a concrete implementation of a scene manager will 
	 * register its creation method using setFactoryMethod() which will then later be used for creating the scene manager
	 * during application start up.
	 */
	class BS_CORE_EXPORT SceneManagerFactory
	{
	public:
		/**	Creates a concrete scene manager, depending on the currently set factory method. */
		static void create();

		/**	Sets method that will be used for creating the scene manager when create() gets called. */
		static void setFactoryMethod(const std::function<void()>& method)
		{
			mFactoryMethod = method;
		}

	private:
		static std::function<void()> mFactoryMethod;
	};

	/**	Provides easy access to the CoreSceneManager. */
	BS_CORE_EXPORT CoreSceneManager& gCoreSceneManager();

	/** @} */
	/** @endcond */
}