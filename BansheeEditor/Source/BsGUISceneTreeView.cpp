#include "BsGUISceneTreeView.h"
#include "BsSceneObject.h"
#include "BsSceneManager.h"
#include "BsGUISkin.h"
#include "BsCmdEditPlainFieldGO.h"
#include "BsDragAndDropManager.h"
#include "BsCmdReparentSO.h"
#include "BsSelection.h"

namespace BansheeEngine
{
	const MessageId GUISceneTreeView::SELECTION_CHANGED_MSG = MessageId("SceneTreeView_SelectionChanged");

	DraggedSceneObjects::DraggedSceneObjects(UINT32 numObjects)
		:numObjects(numObjects)
	{
		objects = bs_newN<HSceneObject>(numObjects);
	}

	DraggedSceneObjects::~DraggedSceneObjects()
	{
		bs_deleteN(objects, numObjects);
		objects = nullptr;
	}

	GUISceneTreeView::GUISceneTreeView(const String& backgroundStyle, const String& elementBtnStyle, 
		const String& foldoutBtnStyle, const String& selectionBackgroundStyle, const String& editBoxStyle, 
		const String& dragHighlightStyle, const String& dragSepHighlightStyle, const GUILayoutOptions& layoutOptions)
		:GUITreeView(backgroundStyle, elementBtnStyle, foldoutBtnStyle, selectionBackgroundStyle, editBoxStyle, dragHighlightStyle,
		dragSepHighlightStyle, layoutOptions)
	{
		SceneTreeViewLocator::_provide(this);
	}

	GUISceneTreeView::~GUISceneTreeView()
	{
		SceneTreeViewLocator::_provide(nullptr);
	}

	GUISceneTreeView* GUISceneTreeView::create(const String& backgroundStyle, const String& elementBtnStyle, 
		const String& foldoutBtnStyle, const String& selectionBackgroundStyle, const String& editBoxStyle, const String& dragHighlightStyle, 
		const String& dragSepHighlightStyle)
	{
		return new (bs_alloc<GUISceneTreeView, PoolAlloc>()) GUISceneTreeView(backgroundStyle, elementBtnStyle, foldoutBtnStyle, 
			selectionBackgroundStyle, editBoxStyle, dragHighlightStyle, dragSepHighlightStyle, GUILayoutOptions::create());
	}

	GUISceneTreeView* GUISceneTreeView::create(const GUIOptions& options, const String& backgroundStyle,
		const String& elementBtnStyle, const String& foldoutBtnStyle, const String& selectionBackgroundStyle, 
		const String& editBoxStyle, const String& dragHighlightStyle, const String& dragSepHighlightStyle)
	{
		return new (bs_alloc<GUISceneTreeView, PoolAlloc>()) GUISceneTreeView(backgroundStyle, elementBtnStyle, 
			foldoutBtnStyle, selectionBackgroundStyle, editBoxStyle, dragHighlightStyle, dragSepHighlightStyle, GUILayoutOptions::create(options));
	}

	void GUISceneTreeView::updateTreeElement(SceneTreeElement* element)
	{
		HSceneObject currentSO = element->mSceneObject;

		// Check if SceneObject has changed in any way and update the tree element

		bool completeMatch = (UINT32)element->mChildren.size() == currentSO->getNumChildren();

		// Early exit case - Most commonly there will be no changes between active and cached data so 
		// we first do a quick check in order to avoid expensive comparison later
		if(completeMatch)
		{
			for(UINT32 i = 0; i < currentSO->getNumChildren(); i++)
			{
				SceneTreeElement* currentChild = static_cast<SceneTreeElement*>(element->mChildren[i]);

				UINT64 curId = currentSO->getChild(i)->getInstanceId();
				if(curId != currentChild->mId)
				{
					completeMatch = false;
					break;
				}
			}
		}

		// Not a complete match, compare everything and insert/delete elements as needed
		bool needsUpdate = false;
		if(!completeMatch)
		{
			Vector<TreeElement*> newChildren;

			bool* tempToDelete = (bool*)stackAlloc(sizeof(bool) * (UINT32)element->mChildren.size());
			for(UINT32 i = 0; i < (UINT32)element->mChildren.size(); i++)
				tempToDelete[i] = true;

			for(UINT32 i = 0; i < currentSO->getNumChildren(); i++)
			{
				HSceneObject currentSOChild = currentSO->getChild(i);
				UINT64 curId = currentSOChild->getInstanceId();
				bool found = false;
				for(UINT32 j = 0; j < element->mChildren.size(); j++)
				{
					SceneTreeElement* currentChild = static_cast<SceneTreeElement*>(element->mChildren[j]);

					if(curId == currentChild->mId)
					{
						tempToDelete[j] = false;
						currentChild->mSortedIdx = (UINT32)newChildren.size();
						newChildren.push_back(currentChild);

						found = true;
						break;
					}
				}

				if(!found)
				{
					SceneTreeElement* newChild = bs_new<SceneTreeElement>();
					newChild->mParent = element;
					newChild->mSceneObject = currentSOChild;
					newChild->mId = currentSOChild->getInstanceId();
					newChild->mName = currentSOChild->getName();
					newChild->mSortedIdx = (UINT32)newChildren.size();
					newChild->mIsVisible = element->mIsVisible && element->mIsExpanded;

					newChildren.push_back(newChild);

					updateElementGUI(newChild);
				}
			}

			for(UINT32 i = 0; i < element->mChildren.size(); i++)
			{
				if(!tempToDelete[i])
					continue;

				deleteTreeElementInternal(element->mChildren[i]);
			}

			stackDeallocLast(tempToDelete);

			element->mChildren = newChildren;
			needsUpdate = true;
		}

		// Check if name needs updating
		const String& name = element->mSceneObject->getName();
		if(element->mName != name)
		{
			element->mName = name;
			needsUpdate = true;	
		}

		if(needsUpdate)
			updateElementGUI(element);

		// Calculate the sorted index of the element based on its name
		TreeElement* parent = element->mParent;
		if(parent != nullptr)
		{
			for(UINT32 i = 0; i < (UINT32)parent->mChildren.size(); i++)
			{
				INT32 stringCompare = element->mName.compare(parent->mChildren[i]->mName);
				if(stringCompare > 0)
				{
					if(element->mSortedIdx < parent->mChildren[i]->mSortedIdx)
						std::swap(element->mSortedIdx, parent->mChildren[i]->mSortedIdx);
				}
				else if(stringCompare < 0)
				{
					if(element->mSortedIdx > parent->mChildren[i]->mSortedIdx)
						std::swap(element->mSortedIdx, parent->mChildren[i]->mSortedIdx);
				}
			}
		}

		for(UINT32 i = 0; i < (UINT32)element->mChildren.size(); i++)
		{
			SceneTreeElement* sceneElement = static_cast<SceneTreeElement*>(element->mChildren[i]);
			updateTreeElement(sceneElement);
		}
	}

	void GUISceneTreeView::updateTreeElementHierarchy()
	{
		HSceneObject root = gCoreSceneManager().getRootNode();
		mRootElement.mSceneObject = root;
		mRootElement.mId = root->getInstanceId();
		mRootElement.mSortedIdx = 0;
		mRootElement.mIsExpanded = true;

		updateTreeElement(&mRootElement);
	}

	void GUISceneTreeView::renameTreeElement(GUITreeView::TreeElement* element, const WString& name)
	{
		SceneTreeElement* sceneTreeElement = static_cast<SceneTreeElement*>(element);
		CmdEditPlainFieldGO<String>::execute(sceneTreeElement->mSceneObject, "mName", toString(name));
	}

	void GUISceneTreeView::deleteTreeElement(TreeElement* element)
	{
		// TODO - Actually delete the scene object
	}

	void GUISceneTreeView::deleteTreeElementInternal(GUITreeView::TreeElement* element)
	{
		closeTemporarilyExpandedElements(); // In case this element is one of them

		if(element->mIsSelected)
			unselectElement(element);

		bs_delete(element);
	}

	bool GUISceneTreeView::acceptDragAndDrop() const
	{
		return DragAndDropManager::instance().isDragInProgress() && DragAndDropManager::instance().getDragTypeId() == (UINT32)DragAndDropType::SceneObject;
	}

	void GUISceneTreeView::dragAndDropStart()
	{
		DraggedSceneObjects* draggedSceneObjects = bs_new<DraggedSceneObjects>((UINT32)mSelectedElements.size());

		UINT32 cnt = 0;
		for(auto& selectedElement : mSelectedElements)
		{
			SceneTreeElement* sceneTreeElement = static_cast<SceneTreeElement*>(selectedElement.element);
			draggedSceneObjects->objects[cnt] = sceneTreeElement->mSceneObject;
			cnt++;
		}

		DragAndDropManager::instance().startDrag((UINT32)DragAndDropType::SceneObject, (void*)draggedSceneObjects, 
			std::bind(&GUISceneTreeView::dragAndDropFinalize, this), true);
	}

	void GUISceneTreeView::dragAndDropEnded(TreeElement* overTreeElement)
	{
		if(overTreeElement != nullptr)
		{
			DraggedSceneObjects* draggedSceneObjects = reinterpret_cast<DraggedSceneObjects*>(DragAndDropManager::instance().getDragData());

			Vector<HSceneObject> sceneObjects;
			SceneTreeElement* sceneTreeElement = static_cast<SceneTreeElement*>(overTreeElement);
			HSceneObject newParent = sceneTreeElement->mSceneObject;

			for(UINT32 i = 0; i < draggedSceneObjects->numObjects; i++)
			{
				if(draggedSceneObjects->objects[i] != newParent)
					sceneObjects.push_back(draggedSceneObjects->objects[i]);
			}

			CmdReparentSO::execute(sceneObjects, newParent);
		}
	}

	void GUISceneTreeView::dragAndDropFinalize()
	{
		mDragInProgress = false;
		markContentAsDirty();

		DraggedSceneObjects* draggedSceneObjects = reinterpret_cast<DraggedSceneObjects*>(DragAndDropManager::instance().getDragData());
		bs_delete(draggedSceneObjects);
	}

	bool GUISceneTreeView::_acceptDragAndDrop(const Vector2I position, UINT32 typeId) const
	{
		return typeId == (UINT32)DragAndDropType::SceneObject;
	}

	void GUISceneTreeView::selectionChanged()
	{
		onSelectionChanged();
		sendMessage(SELECTION_CHANGED_MSG);
	}

	Vector<HSceneObject> GUISceneTreeView::getSelection() const
	{
		Vector<HSceneObject> selectedSOs;
		for (auto& selectedElem : mSelectedElements)
		{
			SceneTreeElement* sceneTreeElement = static_cast<SceneTreeElement*>(selectedElem.element);

			selectedSOs.push_back(sceneTreeElement->mSceneObject);
		}

		return selectedSOs;
	}

	void GUISceneTreeView::setSelection(const Vector<HSceneObject>& objects)
	{
		unselectAll();

		SceneTreeElement& root = mRootElement;

		Stack<SceneTreeElement*> todo;
		todo.push(&mRootElement);

		while (!todo.empty())
		{
			SceneTreeElement* currentElem = todo.top();
			todo.pop();

			auto iterFind = std::find(objects.begin(), objects.end(), currentElem->mSceneObject);
			if (iterFind != objects.end())
			{
				expandToElement(currentElem);
				selectElement(currentElem);
			}

			for (auto& child : currentElem->mChildren)
			{
				SceneTreeElement* sceneChild = static_cast<SceneTreeElement*>(child);
				todo.push(sceneChild);
			}
		}
	}

	const String& GUISceneTreeView::getGUITypeName()
	{
		static String typeName = "SceneTreeView";
		return typeName;
	}
}