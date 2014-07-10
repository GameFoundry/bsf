//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsGameObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Manages all objects in the scene and provides various query methods
	 * 			for finding objects. This is just the base class with basic query 
	 *			functionality. You should override it with your own version that
	 * 			implements a spatial data structure of your choice for faster queries.
	 */
	class BS_CORE_EXPORT CoreSceneManager : public Module<CoreSceneManager>
	{
	public:
		CoreSceneManager();
		virtual ~CoreSceneManager();

		/**
		 * @brief	Returns the root scene object.
		 */
		HSceneObject getRootNode() const { return mRootNode; }

		/**
		 * @brief	Called every frame.
		 *
		 * @note	Internal method.
		 */
		virtual void _update();

	protected:
		friend class SceneObject;

		/**
		 * @brief	Register a new node in the scene manager, on the top-most level of the hierarchy.
		 * 			
		 * @note	After you add a node in the scene manager, it takes ownership of its memory and is responsible for releasing it.
		 * 			Do NOT add nodes that have already been added (i.e. if you just want to change their parent). Normally this method will only be called by SceneObject.
		 *
		 * @param [in]	node	Node you wish to add. It's your responsibility not to add duplicate or null nodes. This method won't check.
		 */
		void registerNewSO(const HSceneObject& node);

		/**
		 * @brief	SceneObjects call this when they have a component added to them.
		 */
		virtual void notifyComponentAdded(const HComponent& component);

		/**
		 * @brief	SceneObjects call this when they have a component removed from them.
		 */
		virtual void notifyComponentRemoved(const HComponent& component);

	protected:
		HSceneObject mRootNode;
	};

	/**
	 * @brief	Provides easy access to the scene manager.
	 */
	BS_CORE_EXPORT CoreSceneManager& gSceneManager();
}