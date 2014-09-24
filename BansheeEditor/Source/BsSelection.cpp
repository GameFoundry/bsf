#include "BsSelection.h"
#include "BsGUISceneTreeView.h"
#include "BsGUIResourceTreeView.h"
#include "BsProjectLibrary.h"
#include "BsProjectResourceMeta.h"

namespace BansheeEngine
{
	Selection::Selection(GUISceneTreeView* sceneTreeView, GUIResourceTreeView* resourceTreeView)
		:mSceneTreeView(sceneTreeView), mResourceTreeView(resourceTreeView)
	{
		mSceneSelectionChangedConn = mSceneTreeView->onSelectionChanged.connect(std::bind(&Selection::sceneSelectionChanged, this));
		mResourceSelectionChangedConn = mResourceTreeView->onSelectionChanged.connect(std::bind(&Selection::resourceSelectionChanged, this));
	}

	Selection::~Selection()
	{
		mSceneSelectionChangedConn.disconnect();
		mResourceSelectionChangedConn.disconnect();
	}

	const Vector<HSceneObject>& Selection::getSceneObjects() const
	{
		return mSelectedSceneObjects;
	}

	void Selection::setSceneObjects(const Vector<HSceneObject>& sceneObjects)
	{
		mSelectedSceneObjects = sceneObjects;
		mResourceTreeView->setSelection(mSelectedResourcePaths);
	}

	const Vector<Path>& Selection::getResourcePaths() const
	{
		return mSelectedResourcePaths;
	}

	void Selection::setResourcePaths(const Vector<Path>& paths)
	{
		mSelectedResourcePaths = paths;
	}

	Vector<String> Selection::getResourceUUIDs() const
	{
		Vector<String> UUIDs;
		for (auto& path : mSelectedResourcePaths)
		{
			ProjectLibrary::LibraryEntry* entry = ProjectLibrary::instance().findEntry(path);
			if (entry != nullptr && entry->type == ProjectLibrary::LibraryEntryType::File)
			{
				ProjectLibrary::ResourceEntry* resEntry = static_cast<ProjectLibrary::ResourceEntry*>(entry);
				UUIDs.push_back(resEntry->meta->getUUID());
			}
		}

		return UUIDs;
	}

	void Selection::setResourceUUIDs(const Vector<String>& UUIDs)
	{
		mSelectedResourcePaths.clear();
		for (auto& uuid : UUIDs)
		{
			Path path = ProjectLibrary::instance().uuidToPath(uuid);
			if (path != Path::BLANK)
				mSelectedResourcePaths.push_back(path);
		}

		mResourceTreeView->setSelection(mSelectedResourcePaths);
	}

	void Selection::sceneSelectionChanged()
	{
		mSelectedSceneObjects = mSceneTreeView->getSelection();
	}

	void Selection::resourceSelectionChanged()
	{
		mSelectedResourcePaths = mResourceTreeView->getSelection();
	}
}