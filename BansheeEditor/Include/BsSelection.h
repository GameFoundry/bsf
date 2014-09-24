#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT Selection : public Module<Selection>
	{
	public:
		Selection(GUISceneTreeView* sceneTreeView, GUIResourceTreeView* resourceTreeView);
		~Selection();

		const Vector<HSceneObject>& getSceneObjects() const;
		void setSceneObjects(const Vector<HSceneObject>& sceneObjects);

		const Vector<Path>& getResourcePaths() const;
		void setResourcePaths(const Vector<Path>& paths);

		Vector<String> getResourceUUIDs() const;
		void setResourceUUIDs(const Vector<String>& UUIDs);

	private:
		void sceneSelectionChanged();
		void resourceSelectionChanged();

		GUISceneTreeView* mSceneTreeView;
		GUIResourceTreeView* mResourceTreeView;

		Vector<HSceneObject> mSelectedSceneObjects;
		Vector<Path> mSelectedResourcePaths;

		HEvent mSceneSelectionChangedConn;
		HEvent mResourceSelectionChangedConn;
	};
}