//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsEditorPrerequisites.h"
#include "Math/BsAABox.h"

namespace bs
{
	/** @addtogroup Utility-Editor
	 *  @{
	 */

	/** Possible type of platform file dialogs. */
	enum class FileDialogType
	{
		OpenFile = 0x0,
		OpenFolder = 0x1,
		Save = 0x2,
		Multiselect = 0x10000,
		TypeMask = 0xFFFF
	};

	/**	Contains miscellaneous helper methods. */
	class BS_ED_EXPORT EditorUtility
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
		 * Calculates world space bounds of the specified scene object. This will consider components with bounds like
		 * Renderable.
		 */
		static AABox calculateBounds(const HSceneObject& object);

		/**
		 * Calculates world space bounds of the specified scene objects. This will consider components with bounds like
		 * Renderable.
		 */
		static AABox calculateBounds(const Vector<HSceneObject>& objects);

		/** Calculates world space center of the specified scene objects. */
		static Vector3 calculateCenter(const Vector<HSceneObject>& objects);

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

		/**
		 * Displays a platform specific file/folder open/save dialog.
		 *
		 * @param[in]	type		Type of dialog to open.
		 * @param[in]	defaultPath	Initial path the dialog will be set to once opened.
		 * @param[in]	filterList	Semi-colon separated list of file names or types to display in the dialog,
		 *							for example "*.exe;*.txt;*.png". Ignored if dialog is to display folders instead of
		 *							files.
		 * @param[out]	paths		Output list of selected file or folder paths (if any).
		 * @return					True if file was selected and false if selection was canceled.
		 */
		static bool openBrowseDialog(FileDialogType type, const Path& defaultPath, const String& filterList,
									 Vector<Path>& paths);

	private:
		/**
		 * Retrieves all components containing meshes on the specified object and outputs their bounds.
		 *
		 * @param[in]	object	Object to calculate bounds for.
		 * @param[in]	bounds	Output bounds, if successful.
		 * @return				True if a mesh component was found, otherwise false (bounds will not be updated).
		 */
		static bool calculateMeshBounds(const HSceneObject& object, AABox& bounds);
	};

	/** @} */
}