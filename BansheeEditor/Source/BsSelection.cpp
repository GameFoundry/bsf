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
		mSelectedResourcePaths.clear();

		updateTreeViews();

		onSelectionChanged(mSelectedSceneObjects, Vector<Path>());
	}

	const Vector<Path>& Selection::getResourcePaths() const
	{
		return mSelectedResourcePaths;
	}

	void Selection::setResourcePaths(const Vector<Path>& paths)
	{
		mSelectedResourcePaths = paths;
		mSelectedSceneObjects.clear();

		updateTreeViews();

		onSelectionChanged(Vector<HSceneObject>(), mSelectedResourcePaths);
	}

	Vector<String> Selection::getResourceUUIDs() const
	{
		Vector<String> UUIDs;
		for (auto& path : mSelectedResourcePaths)
		{
			ProjectLibrary::LibraryEntry* entry = gProjectLibrary().findEntry(path);
			if (entry != nullptr && entry->type == ProjectLibrary::LibraryEntryType::File)
			{
				ProjectLibrary::ResourceEntry* resEntry = static_cast<ProjectLibrary::ResourceEntry*>(entry);

				if (resEntry->meta != nullptr)
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
			Path path = gProjectLibrary().uuidToPath(uuid);
			if (path != Path::BLANK)
				mSelectedResourcePaths.push_back(path);
		}

		mSelectedSceneObjects.clear();
		updateTreeViews();

		onSelectionChanged(Vector<HSceneObject>(), mSelectedResourcePaths);
	}

	void Selection::clearSceneSelection()
	{
		setSceneObjects({});
	}

	void Selection::clearResourceSelection()
	{
		setResourceUUIDs({});
	}

	void Selection::ping(const HSceneObject& sceneObject)
	{
		GUISceneTreeView* sceneTreeView = SceneTreeViewLocator::instance();
		if (sceneTreeView != nullptr)
			sceneTreeView->ping(sceneObject);

		onSceneObjectPing(sceneObject);
	}

	void Selection::ping(const Path& resourcePath)
	{
		onResourcePing(resourcePath);
	}

	void Selection::sceneSelectionChanged()
	{
		GUISceneTreeView* sceneTreeView = SceneTreeViewLocator::instance();
		if (sceneTreeView != nullptr)
		{
			Vector<HSceneObject> newSelection = sceneTreeView->getSelection();

			bool isDirty = newSelection.size() != mSelectedSceneObjects.size();
			if (!isDirty)
			{
				UINT32 count = (UINT32)newSelection.size();

				for (UINT32 i = 0; i < count; i++)
				{
					if (newSelection[i] != mSelectedSceneObjects[i])
					{
						isDirty = true;
						break;
					}
				}
			}

			if (!isDirty)
				return;

			mSelectedSceneObjects = newSelection;
			mSelectedResourcePaths.clear();

			onSelectionChanged(mSelectedSceneObjects, Vector<Path>());
		}
	}

	void Selection::resourceSelectionChanged()
	{
		GUIResourceTreeView* resourceTreeView = ResourceTreeViewLocator::instance();
		if (resourceTreeView != nullptr)
		{
			Vector<Path> newSelection = resourceTreeView->getSelection();

			bool isDirty = newSelection.size() != mSelectedResourcePaths.size();
			if (!isDirty)
			{
				UINT32 count = (UINT32)newSelection.size();

				for (UINT32 i = 0; i < count; i++)
				{
					if (newSelection[i] != mSelectedResourcePaths[i])
					{
						isDirty = true;
						break;
					}
				}
			}

			if (!isDirty)
				return;

			mSelectedResourcePaths = newSelection;
			mSelectedSceneObjects.clear();

			onSelectionChanged(Vector<HSceneObject>(), mSelectedResourcePaths);
		}
	}

	void Selection::updateTreeViews()
	{
		GUIResourceTreeView* resourceTreeView = ResourceTreeViewLocator::instance();
		if (resourceTreeView != nullptr)
		{
			// Copy in case setSelection modifies the original.
			Vector<Path> copy = mSelectedResourcePaths;

			resourceTreeView->setSelection(copy);
		}

		GUISceneTreeView* sceneTreeView = SceneTreeViewLocator::instance();
		if (sceneTreeView != nullptr)
		{
			// Copy in case setSelection modifies the original.
			Vector<HSceneObject> copy = mSelectedSceneObjects;

			sceneTreeView->setSelection(copy);
		}
	}
}