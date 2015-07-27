#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT Selection : public Module<Selection>
	{
	public:
		Selection();
		~Selection();

		const Vector<HSceneObject>& getSceneObjects() const;
		void setSceneObjects(const Vector<HSceneObject>& sceneObjects);

		const Vector<Path>& getResourcePaths() const;
		void setResourcePaths(const Vector<Path>& paths);

		Vector<String> getResourceUUIDs() const;
		void setResourceUUIDs(const Vector<String>& UUIDs);

		void clearSceneSelection();
		void clearResourceSelection();

		Event<void(const Vector<HSceneObject>&, const Vector<Path>&)> onSelectionChanged;
	private:
		void sceneSelectionChanged();
		void resourceSelectionChanged();

		Vector<HSceneObject> mSelectedSceneObjects;
		Vector<Path> mSelectedResourcePaths;

		HMessage mSceneSelectionChangedConn;
		HMessage mResourceSelectionChangedConn;
	};
}