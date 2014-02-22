#include "BsGUISceneTreeView.h"
#include "CmSceneObject.h"
#include "CmSceneManager.h"
#include "BsGUISkin.h"
#include "BsCmdEditPlainFieldGO.h"
#include "BsDragAndDropManager.h"
#include "BsCmdReparentSO.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	GUISceneTreeView::DraggedSceneObjects::DraggedSceneObjects(UINT32 numObjects)
		:numObjects(numObjects)
	{
		objects = cm_newN<HSceneObject>(numObjects);
	}

	GUISceneTreeView::DraggedSceneObjects::~DraggedSceneObjects()
	{
		cm_deleteN(objects, numObjects);
		objects = nullptr;
	}

	GUISceneTreeView::GUISceneTreeView(GUIWidget& parent, GUIElementStyle* backgroundStyle, GUIElementStyle* elementBtnStyle, 
		GUIElementStyle* foldoutBtnStyle, GUIElementStyle* selectionBackgroundStyle, GUIElementStyle* editBoxStyle, 
		BS::GUIElementStyle* dragHighlightStyle, BS::GUIElementStyle* dragSepHighlightStyle, const GUILayoutOptions& layoutOptions)
		:GUITreeView(parent, backgroundStyle, elementBtnStyle, foldoutBtnStyle, selectionBackgroundStyle, editBoxStyle, dragHighlightStyle,
		dragSepHighlightStyle, layoutOptions)
	{
		
	}

	GUISceneTreeView::~GUISceneTreeView()
	{
		
	}

	GUISceneTreeView* GUISceneTreeView::create(GUIWidget& parent, GUIElementStyle* backgroundStyle, GUIElementStyle* elementBtnStyle, 
		GUIElementStyle* foldoutBtnStyle, GUIElementStyle* selectionBackgroundStyle, GUIElementStyle* editBoxStyle, GUIElementStyle* dragHighlightStyle, 
		GUIElementStyle* dragSepHighlightStyle)
	{
		return new (cm_alloc<GUISceneTreeView, PoolAlloc>()) GUISceneTreeView(parent, backgroundStyle, elementBtnStyle, foldoutBtnStyle, 
			selectionBackgroundStyle, editBoxStyle, dragHighlightStyle, dragSepHighlightStyle, GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	GUISceneTreeView* GUISceneTreeView::create(GUIWidget& parent, const GUIOptions& options, GUIElementStyle* backgroundStyle,
		GUIElementStyle* elementBtnStyle, GUIElementStyle* foldoutBtnStyle, GUIElementStyle* selectionBackgroundStyle, 
		GUIElementStyle* editBoxStyle, GUIElementStyle* dragHighlightStyle, GUIElementStyle* dragSepHighlightStyle)
	{
		return new (cm_alloc<GUISceneTreeView, PoolAlloc>()) GUISceneTreeView(parent, backgroundStyle, elementBtnStyle, 
			foldoutBtnStyle, selectionBackgroundStyle, editBoxStyle, dragHighlightStyle, dragSepHighlightStyle, GUILayoutOptions::create(options, &GUISkin::DefaultStyle));
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

				UINT32 curId = currentSO->getChild(i)->getId();
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
			Vector<TreeElement*>::type newChildren;

			bool* tempToDelete = (bool*)stackAlloc(sizeof(bool) * (UINT32)element->mChildren.size());
			for(UINT32 i = 0; i < (UINT32)element->mChildren.size(); i++)
				tempToDelete[i] = true;

			for(UINT32 i = 0; i < currentSO->getNumChildren(); i++)
			{
				HSceneObject currentSOChild = currentSO->getChild(i);
				UINT32 curId = currentSOChild->getId();
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
					SceneTreeElement* newChild = cm_new<SceneTreeElement>();
					newChild->mParent = element;
					newChild->mSceneObject = currentSOChild;
					newChild->mId = currentSOChild->getId();
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

				deleteTreeElement(element->mChildren[i]);
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
		HSceneObject root = CM::gSceneManager().getRootNode();
		mRootElement.mSceneObject = root;
		mRootElement.mId = root->getId();
		mRootElement.mSortedIdx = 0;
		mRootElement.mIsExpanded = true;

		updateTreeElement(&mRootElement);
	}

	void GUISceneTreeView::renameTreeElement(GUITreeView::TreeElement* element, const CM::WString& name)
	{
		SceneTreeElement* sceneTreeElement = static_cast<SceneTreeElement*>(element);
		CmdEditPlainFieldGO<String>::execute(sceneTreeElement->mSceneObject, "mName", toString(name));
	}

	void GUISceneTreeView::deleteTreeElement(GUITreeView::TreeElement* element)
	{
		closeTemporarilyExpandedElements(); // In case this element is one of them

		if(element->mIsSelected)
			unselectElement(element);

		cm_delete(element);
	}

	bool GUISceneTreeView::acceptDragAndDrop() const
	{
		return DragAndDropManager::instance().isDragInProgress() && DragAndDropManager::instance().getDragTypeId() == (UINT32)DragAndDropType::SceneObject;
	}

	void GUISceneTreeView::dragAndDropStart()
	{
		DraggedSceneObjects* draggedSceneObjects = cm_new<DraggedSceneObjects>((UINT32)mSelectedElements.size());

		UINT32 cnt = 0;
		for(auto& selectedElement : mSelectedElements)
		{
			SceneTreeElement* sceneTreeElement = static_cast<SceneTreeElement*>(selectedElement.element);
			draggedSceneObjects->objects[cnt] = sceneTreeElement->mSceneObject;
			cnt++;
		}

		DragAndDropManager::instance().startDrag(HTexture(), (UINT32)DragAndDropType::SceneObject, (void*)draggedSceneObjects, 
			boost::bind(&GUISceneTreeView::dragAndDropFinalize, this));
	}

	void GUISceneTreeView::dragAndDropEnded(TreeElement* overTreeElement)
	{
		if(overTreeElement != nullptr)
		{
			DraggedSceneObjects* draggedSceneObjects = reinterpret_cast<DraggedSceneObjects*>(DragAndDropManager::instance().getDragData());

			Vector<HSceneObject>::type sceneObjects;
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
		cm_delete(draggedSceneObjects);
	}

	const String& GUISceneTreeView::getGUITypeName()
	{
		static String typeName = "SceneTreeView";
		return typeName;
	}
}