//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsBansheeSMPrerequisites.h"
#include "BsSceneManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Default scene manager implementation. Allows you to query
	 *			the scene graph for various uses.
	 *
	 * TODO - Update doc when I implement this properly
	 */
	class BS_SM_EXPORT BansheeSceneManager : public SceneManager
	{
	public:
		BansheeSceneManager() {}
		~BansheeSceneManager() {}

		/**
		 * @copydoc	SceneManager::getAllCameras
		 */
		const Vector<HCamera>& getAllCameras() const { return mCachedCameras; }

		/**
		 * @copydoc	SceneManager::getAllRenderables
		 */
		const Vector<HRenderable>& getAllRenderables() const { return mRenderables; }

		/**
		 * @copydoc	SceneManager
		 */
		void updateRenderableTransforms();

	private:
		/**
		 * @brief	Called by scene objects whenever a new component is added to the scene.
		 */
		void notifyComponentAdded(const HComponent& component);

		/**
		 * @brief	Called by scene objects whenever a new component is destroyed.
		 */
		void notifyComponentRemoved(const HComponent& component);

		Vector<HCamera> mCachedCameras;
		Vector<HRenderable> mRenderables;
	};
}