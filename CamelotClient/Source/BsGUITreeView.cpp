#include "BsGUITreeView.h"
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
#include "BsGUIVirtualButtonEvent.h"
#include "BsGUIScrollArea.h"
#include "BsDragAndDropManager.h"
#include "CmTime.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const UINT32 GUITreeView::ELEMENT_EXTRA_SPACING = 3;
	const UINT32 GUITreeView::INDENT_SIZE = 10;
	const UINT32 GUITreeView::INITIAL_INDENT_OFFSET = 16;
	const UINT32 GUITreeView::DRAG_MIN_DISTANCE = 3;
	const float GUITreeView::AUTO_EXPAND_DELAY_SEC = 0.5f;
	const float GUITreeView::SCROLL_AREA_HEIGHT_PCT = 0.1f;
	const UINT32 GUITreeView::SCROLL_SPEED_PX_PER_SEC = 25;

	VirtualButton GUITreeView::mRenameVB = VirtualButton("Rename");
	VirtualButton GUITreeView::mDeleteVB = VirtualButton("Delete");

	GUITreeView::TreeElement::TreeElement()
		:mParent(nullptr), mFoldoutBtn(nullptr), mElement(nullptr), mIsSelected(false),
		mIsExpanded(false), mSortedIdx(0), mIsVisible(true)
	{ }

	GUITreeView::TreeElement::~TreeElement()
	{
		for(auto& child : mChildren)
			cm_delete(child);

		if(mFoldoutBtn != nullptr)
			GUIElement::destroy(mFoldoutBtn);

		if(mElement != nullptr)
			GUIElement::destroy(mElement);

		mChildren.clear();
	}

	bool GUITreeView::TreeElement::isParentRec(TreeElement* element) const
	{
		TreeElement* curParent = mParent;
		while(curParent != nullptr)
		{
			if(curParent == element)
				return true;

			curParent = curParent->mParent;
		}

		return false;
	}

	GUITreeView::TreeElement* GUITreeView::InteractableElement::getTreeElement() const
	{
		if(!isTreeElement())
			return nullptr;

		UINT32 sortedIdx = (index - 1) / 2;

		auto findIter = std::find_if(parent->mChildren.begin(), parent->mChildren.end(),
			[&](const TreeElement* x) { return x->mSortedIdx == sortedIdx; });

		if(findIter != parent->mChildren.end())
			return *findIter;

		return nullptr;
	}

	GUITreeView::GUITreeView(GUIWidget& parent, GUIElementStyle* backgroundStyle, GUIElementStyle* elementBtnStyle, 
		GUIElementStyle* foldoutBtnStyle, GUIElementStyle* selectionBackgroundStyle, GUIElementStyle* editBoxStyle, 
		BS::GUIElementStyle* dragHighlightStyle, BS::GUIElementStyle* dragSepHighlightStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mBackgroundStyle(backgroundStyle),
		mElementBtnStyle(elementBtnStyle), mFoldoutBtnStyle(foldoutBtnStyle), mEditBoxStyle(editBoxStyle), mEditElement(nullptr), mIsElementSelected(false),
		mNameEditBox(nullptr), mSelectionBackgroundStyle(selectionBackgroundStyle), mDragInProgress(nullptr), mDragHighlightStyle(dragHighlightStyle),
		mDragSepHighlightStyle(dragSepHighlightStyle), mDragHighlight(nullptr), mDragSepHighlight(nullptr), mMouseOverDragElement(nullptr), mMouseOverDragElementTime(0.0f),
		mScrollState(ScrollState::None), mLastScrollTime(0.0f)
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

		mNameEditBox->onInputConfirmed.connect(boost::bind(&GUITreeView::onEditAccepted, this));
		mNameEditBox->onInputCanceled.connect(boost::bind(&GUITreeView::onEditCanceled, this));

		mDragHighlight = GUITexture::create(parent, mDragHighlightStyle);
		mDragSepHighlight = GUITexture::create(parent, mDragSepHighlightStyle);

		mDragHighlight->disableRecursively();
		mDragSepHighlight->disableRecursively();

		_registerChildElement(mBackgroundImage);
		_registerChildElement(mNameEditBox);
		_registerChildElement(mDragHighlight);
		_registerChildElement(mDragSepHighlight);
	}

	GUITreeView::~GUITreeView()
	{

	}

	void GUITreeView::update()
	{
		// Attempt to auto-expand elements we are dragging over
		if(acceptDragAndDrop())
		{
			const GUITreeView::InteractableElement* element = findElementUnderCoord(mDragPosition);
			temporarilyExpandElement(element);
		}

		// NOTE - Instead of iterating through every visible element and comparing it with internal values,
		// I might just want to add callbacks to SceneManager that notify me of any changes and then only perform
		// update if anything is actually dirty

		updateTreeElementHierarchy();

		// Attempt to scroll if needed
		if(mScrollState != ScrollState::None)
		{
			GUIScrollArea* scrollArea = findParentScrollArea();
			if(scrollArea != nullptr)
			{
				float curTime = gTime().getTime();
				float timeDiff = curTime - mLastScrollTime;
				float secondsPerPixel = 1.0f / SCROLL_SPEED_PX_PER_SEC;

				switch(mScrollState)
				{
				case ScrollState::TransitioningUp:
					mScrollState = ScrollState::Up;
					mLastScrollTime = curTime;
					break;
				case ScrollState::TransitioningDown:
					mScrollState = ScrollState::Down;
					mLastScrollTime = curTime;
					break;
				case ScrollState::Up:
					{
						UINT32 scrollAmount = (UINT32)Math::floorToInt(timeDiff / secondsPerPixel);
						mLastScrollTime += scrollAmount * secondsPerPixel;

						scrollArea->scrollUpPx(scrollAmount);
					}
					break;
				case ScrollState::Down:
					{
						UINT32 scrollAmount = (UINT32)Math::floorToInt(timeDiff / secondsPerPixel);
						mLastScrollTime += scrollAmount * secondsPerPixel;

						scrollArea->scrollDownPx(scrollAmount);
					}
					break;
				}
			}
		}
	}

	bool GUITreeView::mouseEvent(const GUIMouseEvent& event)
	{
		if(event.getType() == GUIMouseEventType::MouseUp)
		{
			if(DragAndDropManager::instance().isDragInProgress())
				return false;

			const GUITreeView::InteractableElement* element = findElementUnderCoord(event.getPosition());
			TreeElement* treeElement = nullptr;

			if(element != nullptr && element->isTreeElement())
			{
				treeElement = element->getTreeElement();
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

							TreeElement* curElem = iterStartFind->getTreeElement();
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
										selectElement(iterStartFind->getTreeElement());
								}
							}
							else if(iterEndFind < iterStartFind)
							{
								for(;iterEndFind != (iterStartFind + 1); ++iterEndFind)
								{
									if(iterEndFind->isTreeElement())
										selectElement(iterEndFind->getTreeElement());
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
					const GUITreeView::InteractableElement* element = findElementUnderCoord(mDragStartPosition);
					TreeElement* treeElement = nullptr;

					if(element != nullptr && element->isTreeElement())
					{
						// If element we are trying to drag isn't selected, select it
						TreeElement* treeElement = element->getTreeElement();
						auto iterFind = std::find_if(mSelectedElements.begin(), mSelectedElements.end(), 
							[&] (const SelectedElement& x) { return x.element == treeElement; });

						if(iterFind == mSelectedElements.end())
						{
							unselectAll();
							selectElement(element->getTreeElement());
						}						
					}

					dragAndDropStart();

					mDragPosition = event.getPosition();
					mDragInProgress = true;
					mScrollState = ScrollState::None;
					markContentAsDirty();
				}
			}
		}
		else if(event.getType() == GUIMouseEventType::MouseDragAndDropDragged)
		{
			if(acceptDragAndDrop())
			{
				mDragPosition = event.getPosition();
				mDragInProgress = true;
				markContentAsDirty();

				if(mBottomScrollBounds.contains(mDragPosition))
				{
					if(mScrollState != ScrollState::Down)
						mScrollState = ScrollState::TransitioningDown;
				}
				else if(mTopScrollBounds.contains(mDragPosition))
				{
					if(mScrollState != ScrollState::Up)
						mScrollState = ScrollState::TransitioningUp;
				}
				else
					mScrollState = ScrollState::None;

				return true;
			}
		}
		else if(event.getType() == GUIMouseEventType::MouseDragAndDropDropped)
		{
			if(acceptDragAndDrop())
			{
				const GUITreeView::InteractableElement* element = findElementUnderCoord(event.getPosition());

				TreeElement* treeElement = nullptr;
				if(element != nullptr)
				{
					if(element->isTreeElement())
						treeElement = element->getTreeElement();
					else
						treeElement = element->parent;
				}

				dragAndDropEnded(treeElement);
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

	bool GUITreeView::commandEvent(const GUICommandEvent& ev)
	{
		if(ev.getType() == GUICommandEventType::CursorMoveUp || ev.getType() == GUICommandEventType::SelectUp)
		{
			TreeElement* topMostElement = getTopMostSelectedElement();
			auto topMostIter = std::find_if(mVisibleElements.begin(), mVisibleElements.end(), 
				[&] (const InteractableElement& x) { return x.getTreeElement() == topMostElement; });

			if(topMostIter != mVisibleElements.end() && topMostIter != mVisibleElements.begin())
			{
				do 
				{
					topMostIter--;
				} while (!topMostIter->isTreeElement() && topMostIter != mVisibleElements.begin());

				if(topMostIter->isTreeElement())
				{
					if(ev.getType() == GUICommandEventType::CursorMoveUp)
						unselectAll();

					TreeElement* treeElement = topMostIter->getTreeElement();
					selectElement(treeElement);
					scrollToElement(treeElement, false);
				}
			}
		}
		else if(ev.getType() == GUICommandEventType::CursorMoveDown || ev.getType() == GUICommandEventType::SelectDown)
		{
			TreeElement* bottoMostElement = getBottomMostSelectedElement();
			auto bottomMostIter = std::find_if(mVisibleElements.begin(), mVisibleElements.end(), 
				[&] (const InteractableElement& x) { return x.getTreeElement() == bottoMostElement; });

			if(bottomMostIter != mVisibleElements.end())
			{
				do 
				{
					bottomMostIter++;
				} while (bottomMostIter != mVisibleElements.end() && !bottomMostIter->isTreeElement());

				if(bottomMostIter != mVisibleElements.end() && bottomMostIter->isTreeElement())
				{
					if(ev.getType() == GUICommandEventType::CursorMoveDown)
						unselectAll();

					TreeElement* treeElement = bottomMostIter->getTreeElement();
					selectElement(treeElement);
					scrollToElement(treeElement, false);
				}
			}
		}

		return false;
	}

	bool GUITreeView::virtualButtonEvent(const BS::GUIVirtualButtonEvent& ev)
	{
		if(ev.getButton() == mRenameVB)
		{
			if(isSelectionActive() && mEditElement == nullptr)
			{
				enableEdit(mSelectedElements[0].element);
				unselectAll();
			}

			return true;
		}
		else if(ev.getButton() == mDeleteVB)
		{
			if(isSelectionActive())
			{
				auto isChildOf = [&] (const TreeElement* parent, const TreeElement* child)
				{
					const TreeElement* elem = child;

					while(elem != nullptr && elem != parent)
						elem = child->mParent;

					return elem == parent;
				};

				// Ensure we don't unnecessarily try to delete children if their
				// parent is getting deleted anyway
				Vector<TreeElement*>::type elementsToDelete;
				for(UINT32 i = 0; i < (UINT32)mSelectedElements.size(); i++)
				{
					bool hasDeletedParent = false;
					for(UINT32 j = i + 1; j < (UINT32)mSelectedElements.size(); j++)
					{
						if(isChildOf(mSelectedElements[j].element, mSelectedElements[i].element))
						{
							hasDeletedParent = true;
							break;
						}
					}

					if(!hasDeletedParent)
						elementsToDelete.push_back(mSelectedElements[i].element);
				}

				unselectAll();

				for(auto& elem : elementsToDelete)
					deleteTreeElement(elem);
			}
		}

		return false;
	}

	bool GUITreeView::isSelectionActive() const
	{
		return mIsElementSelected && mSelectedElements.size() > 0;
	}

	void GUITreeView::selectElement(TreeElement* element)
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

	void GUITreeView::unselectElement(TreeElement* element)
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

	void GUITreeView::unselectAll()
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

	void GUITreeView::expandElement(TreeElement* element)
	{
		if(element->mIsExpanded)
			return;

		element->mIsExpanded = true;

		if(element->mParent == nullptr || (element->mParent->mIsVisible && element->mParent->mIsExpanded))
		{
			Stack<TreeElement*>::type todo;
			todo.push(element);

			while(!todo.empty())
			{
				TreeElement* curElem = todo.top();
				todo.pop();

				curElem->mIsVisible = true;
				updateElementGUI(curElem);

				if(curElem->mIsExpanded)
				{
					for(auto& child : curElem->mChildren)
						todo.push(child);
				}
			}
		}
	}

	void GUITreeView::collapseElement(TreeElement* element)
	{
		if(!element->mIsExpanded)
			return;

		element->mIsExpanded = false;
		updateElementGUI(element);

		if(element->mParent == nullptr || (element->mParent->mIsVisible && element->mParent->mIsExpanded))
		{
			Stack<TreeElement*>::type todo;

			for(auto& child : element->mChildren)
				todo.push(child);

			while(!todo.empty())
			{
				TreeElement* curElem = todo.top();
				todo.pop();

				curElem->mIsVisible = false;
				if(curElem->mIsSelected)
					unselectElement(curElem);

				updateElementGUI(curElem);

				if(curElem->mIsExpanded)
				{
					for(auto& child : curElem->mChildren)
						todo.push(child);
				}
			}
		}
	}

	void GUITreeView::updateElementGUI(TreeElement* element)
	{
		if(element == &getRootElement())
			return;

		if(element->mIsVisible)
		{
			HString name(toWString(element->mName));
			if(element->mElement == nullptr)
			{
				element->mElement = GUILabel::create(_getParentWidget(), name, mElementBtnStyle);
				_registerChildElement(element->mElement);
			}

			if(element->mChildren.size() > 0)
			{
				if(element->mFoldoutBtn == nullptr)
				{
					element->mFoldoutBtn = GUIToggle::create(_getParentWidget(), GUIContent(HString(L"")), mFoldoutBtnStyle);
					_registerChildElement(element->mFoldoutBtn);

					element->mFoldoutBtn->onToggled.connect(boost::bind(&GUITreeView::elementToggled, this, element, _1));

					if(element->mIsExpanded)
						element->mFoldoutBtn->toggleOn();
				}
			}
			else
			{
				if(element->mFoldoutBtn != nullptr)
				{
					GUIElement::destroy(element->mFoldoutBtn);
					element->mFoldoutBtn = nullptr;
				}
			}

			element->mElement->setContent(GUIContent(name));
		}
		else
		{
			if(element->mElement != nullptr)
			{
				GUIElement::destroy(element->mElement);
				element->mElement = nullptr;
			}

			if(element->mFoldoutBtn != nullptr)
			{
				GUIElement::destroy(element->mFoldoutBtn);
				element->mFoldoutBtn = nullptr;
			}

			if(element->mIsSelected && element->mIsExpanded)
				unselectElement(element);
		}

		markContentAsDirty();
	}

	void GUITreeView::elementToggled(TreeElement* element, bool toggled)
	{
		if(toggled)
			expandElement(element);
		else
			collapseElement(element);
	}

	void GUITreeView::onEditAccepted()
	{
		disableEdit(true);
	}

	void GUITreeView::onEditCanceled()
	{
		if(mEditElement != nullptr)
			disableEdit(false);
	}

	void GUITreeView::enableEdit(TreeElement* element)
	{
		assert(mEditElement == nullptr);

		mEditElement = element;
		mNameEditBox->enableRecursively();
		mNameEditBox->setFocus(true);

		if(element->mElement != nullptr)
			element->mElement->disableRecursively();
	}

	void GUITreeView::disableEdit(bool applyChanges)
	{
		assert(mEditElement != nullptr);

		if(mEditElement->mElement != nullptr)
			mEditElement->mElement->enableRecursively();

		if(applyChanges)
		{
			WString newName = mNameEditBox->getText();
			renameTreeElement(mEditElement, newName);
		}

		mNameEditBox->disableRecursively();
		mEditElement = nullptr;
	}

	Vector2I GUITreeView::_getOptimalSize() const
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
			todo.push(UpdateTreeElement(&getRootElementConst(), 0));

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

	void GUITreeView::updateClippedBounds()
	{
		Vector2I offset = _getOffset();
		mClippedBounds = RectI(offset.x, offset.y, _getWidth(), _getHeight());

		RectI localClipRect(mClipRect.x + mOffset.x, mClipRect.y + mOffset.y, mClipRect.width, mClipRect.height);
		mClippedBounds.clip(localClipRect);
	}

	void GUITreeView::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
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
		todo.push(UpdateTreeElement(&getRootElement(), 0));

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

				offset.x = x + INITIAL_INDENT_OFFSET + indent * INDENT_SIZE;
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

				offset.x -= std::min((INT32)INITIAL_INDENT_OFFSET, elementSize.x);

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
			mVisibleElements.push_back(InteractableElement(&getRootElement(), (UINT32)getRootElement().mChildren.size() * 2, RectI(x, offset.y, width, remainingHeight)));

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

		// Update scroll bounds
		UINT32 scrollHeight = (UINT32)Math::roundToInt(clipRect.height * SCROLL_AREA_HEIGHT_PCT);

		mTopScrollBounds.x = clipRect.x;
		mTopScrollBounds.y = clipRect.y;
		mTopScrollBounds.width = clipRect.width;
		mTopScrollBounds.height = scrollHeight;

		mBottomScrollBounds.x = clipRect.x;
		mBottomScrollBounds.y = clipRect.y + clipRect.height - scrollHeight;
		mBottomScrollBounds.width = clipRect.width;
		mBottomScrollBounds.height = scrollHeight;
	}

	const GUITreeView::InteractableElement* GUITreeView::findElementUnderCoord(const CM::Vector2I& coord) const
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

	GUITreeView::TreeElement* GUITreeView::getTopMostSelectedElement() const
	{
		auto topMostElement = mVisibleElements.end();

		for(auto& selectedElement : mSelectedElements)
		{
			auto iterFind = std::find_if(mVisibleElements.begin(), mVisibleElements.end(), 
				[&] (const InteractableElement& x) { return x.getTreeElement() == selectedElement.element; });

			if(iterFind != mVisibleElements.end())
			{
				if(topMostElement == mVisibleElements.end())
					topMostElement = iterFind;
				else
				{
					if(iterFind->bounds.y < topMostElement->bounds.y)
						topMostElement = iterFind;
				}
			}
		}

		if(topMostElement != mVisibleElements.end())
			return topMostElement->getTreeElement();
		else
			return nullptr;
	}

	GUITreeView::TreeElement* GUITreeView::getBottomMostSelectedElement() const
	{
		auto& botMostElement = mVisibleElements.end();

		for(auto& selectedElement : mSelectedElements)
		{
			auto iterFind = std::find_if(mVisibleElements.begin(), mVisibleElements.end(), 
				[&] (const InteractableElement& x) { return x.getTreeElement() == selectedElement.element; });

			if(iterFind != mVisibleElements.end())
			{
				if(botMostElement == mVisibleElements.end())
					botMostElement = iterFind;
				else
				{
					if((iterFind->bounds.y + iterFind->bounds.height) > (botMostElement->bounds.y + botMostElement->bounds.height))
						botMostElement = iterFind;
				}
			}
		}

		if(botMostElement != mVisibleElements.end())
			return botMostElement->getTreeElement();
		else
			return nullptr;
	}

	void GUITreeView::closeTemporarilyExpandedElements()
	{
		temporarilyExpandElement(nullptr);
	}

	void GUITreeView::temporarilyExpandElement(const GUITreeView::InteractableElement* mouseOverElement)
	{
		TreeElement* treeElement = nullptr;
		if(mouseOverElement != nullptr && mouseOverElement->isTreeElement())
			treeElement = mouseOverElement->getTreeElement();

		if(treeElement == nullptr || treeElement != mMouseOverDragElement)
		{
			while(!mAutoExpandedElements.empty())
			{
				TreeElement* autoExpandedElement = mAutoExpandedElements.top();

				bool unexpandElement = false;
				if(mouseOverElement != nullptr && mouseOverElement->parent != nullptr)
				{
					if(mouseOverElement->parent != autoExpandedElement && !mouseOverElement->parent->isParentRec(autoExpandedElement))
						unexpandElement = true;
					else
						break;
				}
				else
					unexpandElement = true;

				if(unexpandElement)
				{
					collapseElement(autoExpandedElement);

					if(autoExpandedElement->mFoldoutBtn != nullptr)
						autoExpandedElement->mFoldoutBtn->toggleOff();

					mAutoExpandedElements.pop();
				}
			}

			mMouseOverDragElement = treeElement;
			mMouseOverDragElementTime = gTime().getTime();
		}
		else
		{
			if(mMouseOverDragElement != nullptr && !mMouseOverDragElement->mIsExpanded)
			{
				float timeDiff = gTime().getTime() - mMouseOverDragElementTime;
				if(timeDiff >= AUTO_EXPAND_DELAY_SEC)
				{
					mAutoExpandedElements.push(mMouseOverDragElement);
					expandElement(mMouseOverDragElement);

					if(mMouseOverDragElement->mFoldoutBtn != nullptr)
						mMouseOverDragElement->mFoldoutBtn->toggleOn();
				}
			}
		}
	}

	void GUITreeView::scrollToElement(TreeElement* element, bool center)
	{
		if(element->mElement == nullptr)
			return;

		GUIScrollArea* scrollArea = findParentScrollArea();
		if(scrollArea == nullptr)
			return;

		if(center)
		{
			RectI myBounds = _getClippedBounds();
			INT32 clipVertCenter = myBounds.y + (INT32)Math::roundToInt(myBounds.height * 0.5f);
			INT32 elemVertCenter = element->mElement->_getOffset().y + (INT32)Math::roundToInt(element->mElement->_getHeight() * 0.5f);

			if(elemVertCenter > clipVertCenter)
				scrollArea->scrollUpPx(elemVertCenter - clipVertCenter);
			else
				scrollArea->scrollDownPx(clipVertCenter - elemVertCenter);
		}
		else
		{
			RectI myBounds = _getClippedBounds();
			INT32 elemVertTop = element->mElement->_getOffset().y;
			INT32 elemVertBottom = element->mElement->_getOffset().y + element->mElement->_getHeight();

			INT32 top = myBounds.y;
			INT32 bottom = myBounds.y + myBounds.height;

			INT32 offset = 0;
			if(elemVertTop < top)
				scrollArea->scrollUpPx(top - elemVertTop);
			else if(elemVertBottom > bottom)
				scrollArea->scrollDownPx(elemVertBottom - bottom);
		}
	}

	GUIScrollArea* GUITreeView::findParentScrollArea() const
	{
		GUIElementBase* parent = _getParent();
		while(parent != nullptr)
		{
			if(parent->_getType() == GUIElementBase::Type::Element)
			{
				GUIElement* parentElement = static_cast<GUIElement*>(parent);

				if(parentElement->getElementType() == GUIElement::ElementType::ScrollArea)
				{
					GUIScrollArea* scrollArea = static_cast<GUIScrollArea*>(parentElement);
					return scrollArea;

				}
			}

			parent = parent->_getParent();
		}

		return nullptr;
	}

	const String& GUITreeView::getGUITypeName()
	{
		static String typeName = "SceneTreeView";
		return typeName;
	}
}