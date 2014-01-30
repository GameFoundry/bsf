#include "BsGUISceneTreeView.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUITexture.h"
#include "BsGUIButton.h"
#include "BsGUILabel.h"
#include "BsGUISpace.h"
#include "BsGUIWidget.h"
#include "BsGUIToggle.h"
#include "BsGUITreeViewEditBox.h"
#include "BsGUIMouseEvent.h"
#include "BsGUISkin.h"
#include "BsGUICommandEvent.h"
#include "CmSceneObject.h"
#include "CmSceneManager.h"
#include "BsCmdEditPlainFieldGO.h"
#include "BsDragAndDropManager.h"
#include "BsCmdReparentSO.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const UINT32 GUISceneTreeView::ELEMENT_EXTRA_SPACING = 3;
	const UINT32 GUISceneTreeView::INDENT_SIZE = 10;
	const UINT32 GUISceneTreeView::INITIAL_INDENT_OFFSET = 16;
	const UINT32 GUISceneTreeView::DRAG_MIN_DISTANCE = 3;

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

	GUISceneTreeView::TreeElement::TreeElement()
		:mParent(nullptr), mFoldoutBtn(nullptr), mElement(nullptr), mIsSelected(false),
		mId(0), mIsExpanded(false), mSortedIdx(0), mIsDirty(false), mIsVisible(true)
	{ }

	GUISceneTreeView::TreeElement::~TreeElement()
	{
		for(auto& child : mChildren)
			cm_delete(child);

		if(mFoldoutBtn != nullptr)
			GUIElement::destroy(mFoldoutBtn);

		if(mElement != nullptr)
			GUIElement::destroy(mElement);

		mChildren.clear();
	}

	GUISceneTreeView::GUISceneTreeView(GUIWidget& parent, GUIElementStyle* backgroundStyle, GUIElementStyle* elementBtnStyle, 
		GUIElementStyle* foldoutBtnStyle, GUIElementStyle* selectionBackgroundStyle, GUIElementStyle* editBoxStyle, 
		BS::GUIElementStyle* dragHighlightStyle, BS::GUIElementStyle* dragSepHighlightStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mBackgroundStyle(backgroundStyle),
		mElementBtnStyle(elementBtnStyle), mFoldoutBtnStyle(foldoutBtnStyle), mEditBoxStyle(editBoxStyle), mEditElement(nullptr), mIsElementSelected(false),
		mNameEditBox(nullptr), mSelectionBackgroundStyle(selectionBackgroundStyle), mDragInProgress(nullptr), mDragHighlightStyle(dragHighlightStyle),
		mDragSepHighlightStyle(dragSepHighlightStyle), mDragHighlight(nullptr), mDragSepHighlight(nullptr)
	{
		if(mBackgroundStyle == nullptr)
			mBackgroundStyle = parent.getSkin().getStyle("TreeViewBackground");

		if(mElementBtnStyle == nullptr)
			mElementBtnStyle = parent.getSkin().getStyle("TreeViewElementBtn");

		if(mFoldoutBtnStyle == nullptr)
			mFoldoutBtnStyle = parent.getSkin().getStyle("TreeViewFoldoutBtn");

		if(mSelectionBackgroundStyle == nullptr)
			mSelectionBackgroundStyle = parent.getSkin().getStyle("TreeViewSelectionBackground");

		if(mEditBoxStyle == nullptr)
			mEditBoxStyle = parent.getSkin().getStyle("TreeViewEditBox");

		if(mDragHighlightStyle == nullptr)
			mDragHighlightStyle = parent.getSkin().getStyle("TreeViewElementHighlight");

		if(mDragSepHighlightStyle == nullptr)
			mDragSepHighlightStyle = parent.getSkin().getStyle("TreeViewElementSepHighlight");

		mBackgroundImage = GUITexture::create(parent, mBackgroundStyle);
		mNameEditBox = GUITreeViewEditBox::create(parent, mEditBoxStyle);
		mNameEditBox->disableRecursively();

		mNameEditBox->onInputConfirmed.connect(boost::bind(&GUISceneTreeView::onEditAccepted, this));
		mNameEditBox->onInputCanceled.connect(boost::bind(&GUISceneTreeView::onEditCanceled, this));

		mDragHighlight = GUITexture::create(parent, mDragHighlightStyle);
		mDragSepHighlight = GUITexture::create(parent, mDragSepHighlightStyle);

		mDragHighlight->disableRecursively();
		mDragSepHighlight->disableRecursively();

		_registerChildElement(mBackgroundImage);
		_registerChildElement(mNameEditBox);
		_registerChildElement(mDragHighlight);
		_registerChildElement(mDragSepHighlight);
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

	void GUISceneTreeView::update()
	{
		// NOTE - Instead of iterating through every visible element and comparing it with internal values,
		// I might just want to add callbacks to SceneManager that notify me of any changes and then only perform
		// update if anything is actually dirty

		struct UpdateTreeElement
		{
			UpdateTreeElement(TreeElement* element, UINT32 seqIdx, bool visible)
				:element(element), seqIdx(seqIdx), visible(visible)
			{ }

			TreeElement* element;
			UINT32 seqIdx;
			bool visible;
		};

		HSceneObject root = CM::gSceneManager().getRootNode();
		mRootElement.mSceneObject = root;
		mRootElement.mId = root->getId();
		mRootElement.mSortedIdx = 0;
		mRootElement.mIsExpanded = true;

		Stack<UpdateTreeElement>::type todo;
		todo.push(UpdateTreeElement(&mRootElement, 0, true));

		while(!todo.empty())
		{
			UpdateTreeElement updateElement = todo.top();
			TreeElement* current = updateElement.element;
			HSceneObject currentSO = current->mSceneObject;
			todo.pop();

			// Check if SceneObject has changed in any way and update the tree element
			if(updateElement.visible)
			{
				bool completeMatch = (UINT32)current->mChildren.size() == currentSO->getNumChildren();

				// Early exit case - Most commonly there will be no changes between active and cached data so 
				// we first do a quick check in order to avoid expensive comparison later
				if(completeMatch)
				{
					for(UINT32 i = 0; i < currentSO->getNumChildren(); i++)
					{
						UINT32 curId = currentSO->getChild(i)->getId();
						if(curId != current->mChildren[i]->mId)
						{
							completeMatch = false;
							break;
						}
					}
				}

				// Not a complete match, compare everything and insert/delete elements as needed
				if(!completeMatch)
				{
					Vector<TreeElement*>::type newChildren;

					mTempToDelete.resize(current->mChildren.size());
					for(UINT32 i = 0; i < (UINT32)current->mChildren.size(); i++)
						mTempToDelete[i] = true;

					for(UINT32 i = 0; i < currentSO->getNumChildren(); i++)
					{
						HSceneObject currentSOChild = currentSO->getChild(i);
						UINT32 curId = currentSOChild->getId();
						bool found = false;
						for(UINT32 j = 0; j < current->mChildren.size(); j++)
						{
							TreeElement* currentChild = current->mChildren[j];

							if(curId == currentChild->mId)
							{
								mTempToDelete[j] = false;
								currentChild->mIsDirty = true;
								currentChild->mSortedIdx = (UINT32)newChildren.size();
								newChildren.push_back(currentChild);

								found = true;
								break;
							}
						}

						if(!found)
						{
							TreeElement* newChild = cm_new<TreeElement>();
							newChild->mParent = current;
							newChild->mSceneObject = currentSOChild;
							newChild->mId = currentSOChild->getId();
							newChild->mName = currentSOChild->getName();
							newChild->mSortedIdx = (UINT32)newChildren.size();
							newChild->mIsDirty = true;

							newChildren.push_back(newChild);
						}
					}

					for(UINT32 i = 0; i < current->mChildren.size(); i++)
					{
						if(!mTempToDelete[i])
							continue;

						if(current->mChildren[i]->mIsSelected)
							unselectElement(current->mChildren[i]);

						cm_delete(current->mChildren[i]);
					}

					current->mChildren = newChildren;
					current->mIsDirty = true;
				}

				// Check if name needs updating
				const String& name = current->mSceneObject->getName();
				if(current->mName != name)
				{
					current->mName = name;
					current->mIsDirty = true;		
				}

				// Calculate the sorted index of the element based on its name
				TreeElement* parent = current->mParent;
				if(current->mIsDirty && parent != nullptr)
				{
					for(UINT32 i = 0; i < (UINT32)parent->mChildren.size(); i++)
					{
						INT32 stringCompare = current->mName.compare(parent->mChildren[i]->mName);
						if(stringCompare > 0)
						{
							if(current->mSortedIdx < parent->mChildren[i]->mSortedIdx)
								std::swap(current->mSortedIdx, parent->mChildren[i]->mSortedIdx);
						}
						else if(stringCompare < 0)
						{
							if(current->mSortedIdx > parent->mChildren[i]->mSortedIdx)
								std::swap(current->mSortedIdx, parent->mChildren[i]->mSortedIdx);
						}
					}
				}
			}

			bool visibilityChanged = false;
			if(current->mIsVisible != updateElement.visible)
			{
				visibilityChanged = true;
				current->mIsVisible = updateElement.visible;
				current->mIsDirty = true;
			}
			
			if(current->mIsDirty && current != &mRootElement)
			{
				if(updateElement.visible)
				{
					HString name(toWString(current->mName));
					if(current->mElement == nullptr)
					{
						current->mElement = GUILabel::create(_getParentWidget(), name, mElementBtnStyle);
						_registerChildElement(current->mElement);
					}

					if(current->mChildren.size() > 0)
					{
						if(current->mFoldoutBtn == nullptr)
						{
							current->mFoldoutBtn = GUIToggle::create(_getParentWidget(), GUIContent(HString(L"")), mFoldoutBtnStyle);
							_registerChildElement(current->mFoldoutBtn);

							current->mFoldoutBtn->onToggled.connect(boost::bind(&GUISceneTreeView::elementToggled, this, current, _1));
						}
					}
					else
					{
						if(current->mFoldoutBtn != nullptr)
						{
							GUIElement::destroy(current->mFoldoutBtn);
							current->mFoldoutBtn = nullptr;
						}
					}

					current->mElement->setContent(GUIContent(name));
				}
				else
				{
					if(current->mElement != nullptr)
					{
						GUIElement::destroy(current->mElement);
						current->mElement = nullptr;
					}

					if(current->mFoldoutBtn != nullptr)
					{
						GUIElement::destroy(current->mFoldoutBtn);
						current->mFoldoutBtn = nullptr;
					}

					if(visibilityChanged && current->mIsSelected)
						unselectElement(current);
				}

				markContentAsDirty();
				current->mIsDirty = false;
			}

			// Queue children for next iteration
			if(visibilityChanged || current->mIsVisible)
			{
				for(UINT32 i = 0; i < (UINT32)current->mChildren.size(); i++)
				{
					todo.push(UpdateTreeElement(current->mChildren[i], i, current->mIsVisible && current->mIsExpanded));
				}
			}
		}
	}

	bool GUISceneTreeView::mouseEvent(const GUIMouseEvent& event)
	{
		if(event.getType() == GUIMouseEventType::MouseUp)
		{
			if(DragAndDropManager::instance().isDragInProgress())
				return false;

			const GUISceneTreeView::InteractableElement* element = findElementUnderCoord(event.getPosition());
			TreeElement* treeElement = nullptr;

			if(element != nullptr && element->isTreeElement())
			{
				treeElement = interactableToRealElement(*element);
			}

			if(treeElement != nullptr && event.getPosition().x >= treeElement->mElement->getBounds().x)
			{
				if(event.isCtrlDown())
				{
					selectElement(treeElement);
				}
				else if(event.isShiftDown())
				{
					if(isSelectionActive())
					{
						TreeElement* selectionRoot = mSelectedElements[0].element;
						unselectAll();

						auto iterStartFind = std::find_if(mVisibleElements.begin(), mVisibleElements.end(),
							[&] (const InteractableElement& x) { return x.parent == selectionRoot->mParent; } );

						bool foundStart = false;
						bool foundEnd = false;
						for(; iterStartFind != mVisibleElements.end(); ++iterStartFind)
						{
							if(!iterStartFind->isTreeElement())
								continue;

							TreeElement* curElem = interactableToRealElement(*iterStartFind);
							if(curElem == selectionRoot)
							{
								foundStart = true;
								break;
							}
						}

						auto iterEndFind = std::find_if(mVisibleElements.begin(), mVisibleElements.end(),
							[&] (const InteractableElement& x) { return &x == element; } );

						if(iterEndFind != mVisibleElements.end())
							foundEnd = true;

						if(foundStart && foundEnd)
						{
							if(iterStartFind < iterEndFind)
							{
								for(;iterStartFind != (iterEndFind + 1); ++iterStartFind)
								{
									if(iterStartFind->isTreeElement())
										selectElement(interactableToRealElement(*iterStartFind));
								}
							}
							else if(iterEndFind < iterStartFind)
							{
								for(;iterEndFind != (iterStartFind + 1); ++iterEndFind)
								{
									if(iterEndFind->isTreeElement())
										selectElement(interactableToRealElement(*iterEndFind));
								}
							}
							else
								selectElement(treeElement);
						}

						if(!foundStart || !foundEnd)
							selectElement(treeElement);
					}
					else
					{
						selectElement(treeElement);
					}
				}
				else
				{
					unselectAll();
					selectElement(treeElement);
				}

				markContentAsDirty();

				return true;
			}
		}
		else if(event.getType() == GUIMouseEventType::MouseDragStart)
		{
			mDragStartPosition = event.getPosition();
		}
		else if(event.getType() == GUIMouseEventType::MouseDrag)
		{
			UINT32 dist = mDragStartPosition.manhattanDist(event.getPosition());

			if(!DragAndDropManager::instance().isDragInProgress())
			{
				if(dist > DRAG_MIN_DISTANCE)
				{
					const GUISceneTreeView::InteractableElement* element = findElementUnderCoord(mDragStartPosition);
					TreeElement* treeElement = nullptr;

					if(element != nullptr && element->isTreeElement())
					{
						// If element we are trying to drag isn't selected, select it
						TreeElement* treeElement = interactableToRealElement(*element);
						auto iterFind = std::find_if(mSelectedElements.begin(), mSelectedElements.end(), 
							[&] (const SelectedElement& x) { return x.element == treeElement; });

						if(iterFind == mSelectedElements.end())
						{
							unselectAll();
							selectElement(interactableToRealElement(*element));
						}						
					}

					DraggedSceneObjects* draggedSceneObjects = cm_new<DraggedSceneObjects>((UINT32)mSelectedElements.size());

					UINT32 cnt = 0;
					for(auto& selectedElement : mSelectedElements)
					{
						draggedSceneObjects->objects[cnt] = selectedElement.element->mSceneObject;
						cnt++;
					}

					DragAndDropManager::instance().startDrag(HTexture(), (UINT32)DragAndDropType::SceneObject, (void*)draggedSceneObjects, 
						boost::bind(&GUISceneTreeView::dragAndDropEnded, this));

					mDragPosition = event.getPosition();
					mDragInProgress = true;
					markContentAsDirty();
				}
			}
		}
		else if(event.getType() == GUIMouseEventType::MouseDragAndDropDragged)
		{
			if(DragAndDropManager::instance().isDragInProgress() && DragAndDropManager::instance().getDragTypeId() == (UINT32)DragAndDropType::SceneObject)
			{
				mDragPosition = event.getPosition();
				mDragInProgress = true;
				markContentAsDirty();

				return true;
			}
		}
		else if(event.getType() == GUIMouseEventType::MouseDragAndDropDropped)
		{
			if(DragAndDropManager::instance().isDragInProgress() && DragAndDropManager::instance().getDragTypeId() == (UINT32)DragAndDropType::SceneObject)
			{
				DraggedSceneObjects* draggedSceneObjects = reinterpret_cast<DraggedSceneObjects*>(DragAndDropManager::instance().getDragData());
				const GUISceneTreeView::InteractableElement* element = findElementUnderCoord(event.getPosition());

				TreeElement* treeElement = nullptr;
				if(element != nullptr)
				{
					if(element->isTreeElement())
						treeElement = interactableToRealElement(*element);
					else
						treeElement = element->parent;
				}

				if(treeElement != nullptr)
				{
					Vector<HSceneObject>::type sceneObjects;
					HSceneObject newParent = treeElement->mSceneObject;

					for(UINT32 i = 0; i < draggedSceneObjects->numObjects; i++)
					{
						if(draggedSceneObjects->objects[i] != newParent)
							sceneObjects.push_back(draggedSceneObjects->objects[i]);
					}

					CmdReparentSO::execute(sceneObjects, newParent);
				}

				unselectAll();

				return true;
			}
		}
		else if(event.getType() == GUIMouseEventType::MouseOut)
		{
			mDragInProgress = false;
			markContentAsDirty();
		}

		return false;
	}

	bool GUISceneTreeView::commandEvent(const GUICommandEvent& ev)
	{
		if(ev.getType() == GUICommandEventType::Rename)
		{
			if(isSelectionActive() && mEditElement == nullptr)
			{
				unselectAll();
				enableEdit(mSelectedElements[0].element);
			}

			return true;
		}

		return false;
	}

	void GUISceneTreeView::dragAndDropEnded()
	{
		mDragInProgress = false;
		markContentAsDirty();

		DraggedSceneObjects* draggedSceneObjects = reinterpret_cast<DraggedSceneObjects*>(DragAndDropManager::instance().getDragData());
		cm_delete(draggedSceneObjects);
	}

	bool GUISceneTreeView::isSelectionActive() const
	{
		return mIsElementSelected && mSelectedElements.size() > 0;
	}

	void GUISceneTreeView::selectElement(TreeElement* element)
	{
		auto iterFind = std::find_if(mSelectedElements.begin(), mSelectedElements.end(), 
			[&] (const SelectedElement& x) { return x.element == element; });

		if(iterFind == mSelectedElements.end())
		{
			GUITexture* background = GUITexture::create(_getParentWidget(), mSelectionBackgroundStyle);
			_registerChildElement(background);

			element->mIsSelected = true;

			mSelectedElements.push_back(SelectedElement(element, background));
			mIsElementSelected = true;
		}
	}

	void GUISceneTreeView::unselectElement(TreeElement* element)
	{
		auto iterFind = std::find_if(mSelectedElements.begin(), mSelectedElements.end(), 
			[&] (const SelectedElement& x) { return x.element == element; });

		if(iterFind != mSelectedElements.end())
		{
			iterFind->element->mIsSelected = false;
			GUIElement::destroy(iterFind->background);

			mSelectedElements.erase(iterFind);
			markContentAsDirty();
		}

		mIsElementSelected = mSelectedElements.size() > 0;
	}

	void GUISceneTreeView::unselectAll()
	{
		for(auto& selectedElem : mSelectedElements)
		{
			selectedElem.element->mIsSelected = false;
			GUIElement::destroy(selectedElem.background);
		}

		mSelectedElements.clear();
		mIsElementSelected = false;

		markContentAsDirty();
	}

	void GUISceneTreeView::elementToggled(TreeElement* element, bool toggled)
	{
		element->mIsExpanded = toggled;
	}

	void GUISceneTreeView::onEditAccepted()
	{
		disableEdit(true);
	}

	void GUISceneTreeView::onEditCanceled()
	{
		if(mEditElement != nullptr)
			disableEdit(false);
	}

	void GUISceneTreeView::enableEdit(TreeElement* element)
	{
		assert(mEditElement == nullptr);

		mEditElement = element;
		mNameEditBox->enableRecursively();
		mNameEditBox->setFocus(true);

		if(element->mElement != nullptr)
			element->mElement->disableRecursively();
	}

	void GUISceneTreeView::disableEdit(bool applyChanges)
	{
		assert(mEditElement != nullptr);
		
		if(mEditElement->mElement != nullptr)
			mEditElement->mElement->enableRecursively();

		if(applyChanges)
		{
			String newName = toString(mNameEditBox->getText());
			CmdEditPlainFieldGO<String>::execute(mEditElement->mSceneObject, "mName", newName);
		}

		mNameEditBox->disableRecursively();
		mEditElement = nullptr;
	}

	Vector2I GUISceneTreeView::_getOptimalSize() const
	{
		struct UpdateTreeElement
		{
			UpdateTreeElement(const TreeElement* element, UINT32 indent)
				:element(element), indent(indent)
			{ }

			const TreeElement* element;
			UINT32 indent;
		};

		Vector2I optimalSize;

		if(_getLayoutOptions().fixedWidth && _getLayoutOptions().fixedHeight)
		{
			optimalSize.x = _getLayoutOptions().width;
			optimalSize.y = _getLayoutOptions().height;
		}
		else
		{
			Stack<UpdateTreeElement>::type todo;
			todo.push(UpdateTreeElement(&mRootElement, 0));

			while(!todo.empty())
			{
				UpdateTreeElement currentUpdateElement = todo.top();
				const TreeElement* current = currentUpdateElement.element;
				todo.pop();

				INT32 yOffset = 0;
				if(current->mElement != nullptr)
				{
					Vector2I curOptimalSize = current->mElement->_getOptimalSize();
					optimalSize.x = std::max(optimalSize.x, 
						(INT32)(INITIAL_INDENT_OFFSET + curOptimalSize.x + currentUpdateElement.indent * INDENT_SIZE));
					yOffset = curOptimalSize.y + ELEMENT_EXTRA_SPACING;
				}

				optimalSize.y += yOffset;

				for(auto& child : current->mChildren)
				{
					if(!child->mIsVisible)
						continue;

					todo.push(UpdateTreeElement(child, currentUpdateElement.indent + 1));
				}
			}

			if(_getLayoutOptions().fixedWidth)
				optimalSize.x = _getLayoutOptions().width;
			else
			{
				if(_getLayoutOptions().minWidth > 0)
					optimalSize.x = std::max((INT32)_getLayoutOptions().minWidth, optimalSize.x);

				if(_getLayoutOptions().maxWidth > 0)
					optimalSize.x = std::min((INT32)_getLayoutOptions().maxWidth, optimalSize.x);
			}

			if(_getLayoutOptions().fixedHeight)
				optimalSize.y = _getLayoutOptions().height;
			else
			{
				if(_getLayoutOptions().minHeight > 0)
					optimalSize.y = std::max((INT32)_getLayoutOptions().minHeight, optimalSize.y);

				if(_getLayoutOptions().maxHeight > 0)
					optimalSize.y = std::min((INT32)_getLayoutOptions().maxHeight, optimalSize.y);
			}
		}

		return optimalSize;
	}

	void GUISceneTreeView::updateClippedBounds()
	{
		Vector2I offset = _getOffset();
		mClippedBounds = RectI(offset.x, offset.y, _getWidth(), _getHeight());

		RectI localClipRect(mClipRect.x + mOffset.x, mClipRect.y + mOffset.y, mClipRect.width, mClipRect.height);
		mClippedBounds.clip(localClipRect);
	}

	void GUISceneTreeView::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		struct UpdateTreeElement
		{
			UpdateTreeElement(TreeElement* element, UINT32 indent)
				:element(element), indent(indent)
			{ }

			TreeElement* element;
			UINT32 indent;
		};

		mVisibleElements.clear();

		Stack<UpdateTreeElement>::type todo;
		todo.push(UpdateTreeElement(&mRootElement, 0));

		// NOTE - Instead of iterating through all elements, try to find those within the clip rect
		// and only iterate through those. Others should somehow be marked in-active (similar to GUIElement::isDisabled()?)

		Vector<TreeElement*>::type tempOrderedElements;

		Vector2I offset(x, y);

		while(!todo.empty())
		{
			UpdateTreeElement currentUpdateElement = todo.top();
			TreeElement* current = currentUpdateElement.element;
			UINT32 indent = currentUpdateElement.indent;
			todo.pop();

			INT32 btnHeight = 0;
			INT32 yOffset = 0;
			if(current->mElement != nullptr)
			{
				Vector2I elementSize = current->mElement->_getOptimalSize();
				btnHeight = elementSize.y;

				mVisibleElements.push_back(InteractableElement(current->mParent, current->mSortedIdx * 2 + 0, RectI(x, offset.y, width, ELEMENT_EXTRA_SPACING)));
				mVisibleElements.push_back(InteractableElement(current->mParent, current->mSortedIdx * 2 + 1, RectI(x, offset.y + ELEMENT_EXTRA_SPACING, width, btnHeight)));

				offset.x = INITIAL_INDENT_OFFSET + indent * INDENT_SIZE;
				offset.y += ELEMENT_EXTRA_SPACING;

				current->mElement->_setOffset(offset);
				current->mElement->_setWidth(elementSize.x);
				current->mElement->_setHeight(elementSize.y);
				current->mElement->_setAreaDepth(areaDepth);
				current->mElement->_setWidgetDepth(widgetDepth);

				RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
				current->mElement->_setClipRect(elemClipRect);

				yOffset = btnHeight;
			}

			if(current->mFoldoutBtn != nullptr)
			{
				Vector2I elementSize = current->mFoldoutBtn->_getOptimalSize();

				offset.x -= std::min((INT32)INITIAL_INDENT_OFFSET, elementSize.y);

				Vector2I myOffset = offset;
				myOffset.y -= 2; // TODO: Arbitrary offset, I should adjust it based on font baseline so that the button is nicely centered on text

				if(elementSize.y > btnHeight)
				{
					UINT32 diff = elementSize.y - btnHeight;
					float half = diff * 0.5f;
					myOffset.y -= Math::floorToInt(half);
				}

				current->mFoldoutBtn->_setOffset(myOffset);
				current->mFoldoutBtn->_setWidth(elementSize.x);
				current->mFoldoutBtn->_setHeight(elementSize.y);
				current->mFoldoutBtn->_setAreaDepth(areaDepth);
				current->mFoldoutBtn->_setWidgetDepth(widgetDepth);

				RectI elemClipRect(clipRect.x - myOffset.x, clipRect.y - myOffset.y, clipRect.width, clipRect.height);
				current->mFoldoutBtn->_setClipRect(elemClipRect);
			}

			offset.y += yOffset;

			tempOrderedElements.resize(current->mChildren.size(), nullptr);
			for(auto& child : current->mChildren)
			{
				tempOrderedElements[child->mSortedIdx] = child;
			}

			for(auto iter = tempOrderedElements.rbegin(); iter != tempOrderedElements.rend(); ++iter)
			{
				TreeElement* child = *iter;

				if(!child->mIsVisible)
					continue;

				todo.push(UpdateTreeElement(child, indent + 1));
			}
		}

		UINT32 remainingHeight = (UINT32)std::max(0, (INT32)height - (offset.y - y));

		if(remainingHeight > 0)
			mVisibleElements.push_back(InteractableElement(&mRootElement, (UINT32)mRootElement.mChildren.size() * 2, RectI(x, offset.y, width, remainingHeight)));

		for(auto selectedElem : mSelectedElements)
		{
			GUILabel* targetElement = selectedElem.element->mElement;

			Vector2I offset = targetElement->_getOffset();
			offset.x = x;

			selectedElem.background->_setOffset(offset);
			selectedElem.background->_setWidth(width);
			selectedElem.background->_setHeight(targetElement->_getHeight());
			selectedElem.background->_setAreaDepth(areaDepth + 1);
			selectedElem.background->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			selectedElem.background->_setClipRect(elemClipRect);
		}

		if(mEditElement != nullptr)
		{
			GUILabel* targetElement = mEditElement->mElement;

			Vector2I offset = targetElement->_getOffset();
			UINT32 remainingWidth = (UINT32)std::max(0, (((INT32)width) - (offset.x - x)));

			mNameEditBox->_setOffset(offset);
			mNameEditBox->_setWidth(remainingWidth);
			mNameEditBox->_setHeight(targetElement->_getHeight());
			mNameEditBox->_setAreaDepth(areaDepth);
			mNameEditBox->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mNameEditBox->_setClipRect(elemClipRect);

		}

		if(mDragInProgress)
		{
			const InteractableElement* interactableElement = findElementUnderCoord(mDragPosition);

			if(interactableElement == nullptr)
			{
				if(!mDragHighlight->_isDisabled())
					mDragHighlight->disableRecursively();

				if(!mDragSepHighlight->_isDisabled())
					mDragSepHighlight->disableRecursively();
			}
			else
			{
				if(interactableElement->isTreeElement())
				{
					if(!mDragSepHighlight->_isDisabled())
						mDragSepHighlight->disableRecursively();

					if(mDragHighlight->_isDisabled())
						mDragHighlight->enableRecursively();

					Vector2I offset(interactableElement->bounds.x, interactableElement->bounds.y);
					mDragHighlight->_setOffset(offset);
					mDragHighlight->_setWidth(interactableElement->bounds.width);
					mDragHighlight->_setHeight(interactableElement->bounds.height);
					mDragHighlight->_setAreaDepth(areaDepth + 1);
					mDragHighlight->_setWidgetDepth(widgetDepth);

					RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
					mDragHighlight->_setClipRect(elemClipRect);
				}
				else
				{
					if(!mDragHighlight->_isDisabled())
						mDragHighlight->disableRecursively();

					if(mDragSepHighlight->_isDisabled())
						mDragSepHighlight->enableRecursively();

					Vector2I offset(interactableElement->bounds.x, interactableElement->bounds.y);
					mDragSepHighlight->_setOffset(offset);
					mDragSepHighlight->_setWidth(interactableElement->bounds.width);
					mDragSepHighlight->_setHeight(interactableElement->bounds.height);
					mDragSepHighlight->_setAreaDepth(areaDepth + 1);
					mDragSepHighlight->_setWidgetDepth(widgetDepth);

					RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
					mDragSepHighlight->_setClipRect(elemClipRect);
				}
			}
		}
		else
		{
			if(!mDragHighlight->_isDisabled())
				mDragHighlight->disableRecursively();

			if(!mDragSepHighlight->_isDisabled())
				mDragSepHighlight->disableRecursively();
		}
	}

	const GUISceneTreeView::InteractableElement* GUISceneTreeView::findElementUnderCoord(const CM::Vector2I& coord) const
	{
		for(auto& element : mVisibleElements)
		{
			if(element.bounds.contains(coord))
			{
				return &element;
			}
		}

		return nullptr;
	}

	GUISceneTreeView::TreeElement* GUISceneTreeView::interactableToRealElement(const GUISceneTreeView::InteractableElement& element)
	{
		if(!element.isTreeElement())
			return nullptr;

		UINT32 sortedIdx = (element.index - 1) / 2;

		auto findIter = std::find_if(element.parent->mChildren.begin(), element.parent->mChildren.end(),
			[&](const TreeElement* x) { return x->mSortedIdx == sortedIdx; });

		if(findIter != element.parent->mChildren.end())
			return *findIter;

		return nullptr;
	}

	const String& GUISceneTreeView::getGUITypeName()
	{
		static String typeName = "SceneTreeView";
		return typeName;
	}
}