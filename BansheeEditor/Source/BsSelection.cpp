#include "BsSelection.h"
#include "BsGUISceneTreeView.h"
#include "BsGUIResourceTreeView.h"
#include "BsProjectLibrary.h"
#include "BsProjectResourceMeta.h"
#include "BsMessageHandler.h"

namespace BansheeEngine
{
	Selection::Selection()
	{
		mSceneSelectionChangedConn = MessageHandler::instance().listen(
			GUISceneTreeView::SELECTION_CHANGED_MSG, std::bind(&Selection::sceneSelectionChanged, this));

		mResourceSelectionChangedConn = MessageHandler::instance().listen(
			GUIResourceTreeView::SELECTION_CHANGED_MSG, std::bind(&Selection::resourceSelectionChanged, this));
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

		GUISceneTreeView* sceneTreeView = SceneTreeViewLocator::instance();
		if (sceneTreeView != nullptr)
			sceneTreeView->setSelection(mSelectedSceneObjects);
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

		GUIResourceTreeView* resourceTreeView = ResourceTreeViewLocator::instance();
		if (resourceTreeView != nullptr)
			resourceTreeView->setSelection(mSelectedResourcePaths);
	}

	void Selection::sceneSelectionChanged()
	{
		GUISceneTreeView* sceneTreeView = SceneTreeViewLocator::instance();
		if (sceneTreeView != nullptr)
			mSelectedSceneObjects = sceneTreeView->getSelection();
	}

	void Selection::resourceSelectionChanged()
	{
		GUIResourceTreeView* resourceTreeView = ResourceTreeViewLocator::instance();
		if (resourceTreeView != nullptr)
			mSelectedResourcePaths = resourceTreeView->getSelection();
	}
}