//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsEditorPrerequisites.h"

namespace BansheeEngine
{
	/** @addtogroup UndoRedo
	 *  @{
	 */

	/** Contains various utility methods and structures used by EditorCommand%s. */
	class CmdUtility
	{
	public:
		/**
		 * Contains stored information about stored scene object instance data, including all of its children and
		 * components.
		 *
		 * @note	
		 * When object is serialized it will receive new instance data (as if it was a new object). But we want to restore
		 * the original object completely (including any references	other objects might have to it) so we need store the
		 * instance data.
		 */
		struct SceneObjProxy
		{
			GameObjectInstanceDataPtr instanceData;

			Vector<GameObjectInstanceDataPtr> componentInstanceData;
			Vector<SceneObjProxy> children;
		};

		/**
		 * Parses the scene object hierarchy and components and generates a hierarchy of instance data required to restore
		 * the object identities.
		 */
		static SceneObjProxy createProxy(const HSceneObject& sceneObject);

		/**
		 * Restores original object instance data from the provided scene object proxy that was previously generated using
		 * createProxy().
		 *
		 * @param[in]	restored	New instance of the object.
		 * @param[in]	proxy		Proxy data containing the original object instance data we want to restore.
		 */
		static void restoreIds(const HSceneObject& restored, SceneObjProxy& proxy);
	};

	/** @} */
}