#include "BsGUISceneTreeView.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUITexture.h"
#include "BsGUIButton.h"
#include "BsGUILabel.h"
#include "BsGUISpace.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsGUISkin.h"
#include "CmSceneObject.h"
#include "CmSceneManager.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const UINT32 GUISceneTreeView::ELEMENT_EXTRA_SPACING = 3;
	const UINT32 GUISceneTreeView::INDENT_SIZE = 10;
	const UINT32 GUISceneTreeView::INITIAL_INDENT_OFFSET = 10;

	GUISceneTreeView::TreeElement::TreeElement()
		:mParent(nullptr), mFoldoutBtn(nullptr), mElement(nullptr),
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
		GUIElementStyle* foldoutBtnStyle, const BS::GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mBackgroundStyle(backgroundStyle),
		mElementBtnStyle(elementBtnStyle), mFoldoutBtnStyle(foldoutBtnStyle)
	{
		if(mBackgroundStyle == nullptr)
			mBackgroundStyle = parent.getSkin().getStyle("TreeViewBackground");

		if(mElementBtnStyle == nullptr)
			mElementBtnStyle = parent.getSkin().getStyle("TreeViewElementBtn");

		if(mFoldoutBtnStyle == nullptr)
			mFoldoutBtnStyle = parent.getSkin().getStyle("TreeViewFoldoutBtn");

		mBackgroundImage = GUITexture::create(parent, mBackgroundStyle);
		_registerChildElement(mBackgroundImage);
	}

	GUISceneTreeView::~GUISceneTreeView()
	{

	}

	GUISceneTreeView* GUISceneTreeView::create(GUIWidget& parent, GUIElementStyle* backgroundStyle, GUIElementStyle* elementBtnStyle, 
		GUIElementStyle* foldoutBtnStyle)
	{
		return new (cm_alloc<GUISceneTreeView, PoolAlloc>()) GUISceneTreeView(parent, backgroundStyle, elementBtnStyle, foldoutBtnStyle, 
			GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	GUISceneTreeView* GUISceneTreeView::create(GUIWidget& parent, const GUIOptions& options, GUIElementStyle* backgroundStyle,
		GUIElementStyle* elementBtnStyle, GUIElementStyle* foldoutBtnStyle)
	{
		return new (cm_alloc<GUISceneTreeView, PoolAlloc>()) GUISceneTreeView(parent, backgroundStyle, elementBtnStyle, 
			foldoutBtnStyle, GUILayoutOptions::create(options, &GUISkin::DefaultStyle));
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

					mTempToDelete.resize(current->mChildren.size(), true);
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
						if(current->mSortedIdx <= parent->mChildren[i]->mSortedIdx)
							continue;

						UINT32 stringCompare = current->mName.compare(parent->mChildren[i]->mName);
						if(stringCompare > 0)
						{
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
			
			if(current->mIsDirty)
			{
				if(updateElement.visible)
				{
					if(current->mElement == nullptr)
					{
						HString name(toWString(current->mName));
						current->mElement = GUILabel::create(_getParentWidget(), name);
					}

					// TODO - If no label exists create it
					// TODO - If has children and no expand button exists create it
					// TODO - If it has no children but an expand button exists remove it
				}
				else
				{
					// TODO - If either label or expand button exist remove them
					if(current->mElement != nullptr)
					{
						GUIElement::destroy(current->mElement);
						current->mElement = nullptr;
					}
				}

				markContentAsDirty();
				current->mIsDirty = false;
			}

			// Queue children for next iteration
			if(visibilityChanged || current->mIsVisible)
			{
				for(UINT32 i = 0; i < (UINT32)current->mChildren.size(); i++)
				{
					todo.push(UpdateTreeElement(current->mChildren[i], i, current->mIsVisible /*&& current->mIsExpanded*/)); // TODO - Not checking for mIsExpanded atm
				}
			}
		}
	}

	bool GUISceneTreeView::mouseEvent(const GUIMouseEvent& event)
	{
		

		return false;
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

				if(current->mElement != nullptr)
				{
					Vector2I curOptimalSize = current->mElement->_getOptimalSize();
					optimalSize.x = std::max(optimalSize.x, 
						(INT32)(INITIAL_INDENT_OFFSET + curOptimalSize.x + currentUpdateElement.indent * INDENT_SIZE));
					optimalSize.y += optimalSize.y + ELEMENT_EXTRA_SPACING;
				}

				for(auto& child : current->mChildren)
				{
					todo.push(UpdateTreeElement(child, currentUpdateElement.indent + 1));
				}
			}

			if(_getLayoutOptions().fixedWidth)
				optimalSize.x = _getLayoutOptions().width;
			else
			{
				if(_getLayoutOptions().minWidth > 0)
					optimalSize.x = std::max((INT32)_getLayoutOptions().minWidth, optimalSize.y);

				if(_getLayoutOptions().maxWidth > 0)
					optimalSize.x = std::min((INT32)_getLayoutOptions().maxWidth, optimalSize.y);
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

		Stack<UpdateTreeElement>::type todo;
		todo.push(UpdateTreeElement(&mRootElement, 0));

		// NOTE - Instead of iterating through all elements, try to find those within the clip rect
		// and only iterate through those. Others should somehow be marked in-active (similar to GUIElement::isDisabled()?)

		Vector2I offset(x, y);
		while(!todo.empty())
		{
			UpdateTreeElement currentUpdateElement = todo.top();
			TreeElement* current = currentUpdateElement.element;
			UINT32 indent = currentUpdateElement.indent;
			todo.pop();

			for(auto& child : current->mChildren)
			{
				if(!child->mIsVisible)
					continue;

				if(child->mElement != nullptr)
				{
					Vector2I elementSize = child->mElement->_getOptimalSize();

					offset.x = INITIAL_INDENT_OFFSET + indent * INDENT_SIZE;

					child->mElement->_setOffset(offset);
					child->mElement->_setWidth(elementSize.x);
					child->mElement->_setHeight(elementSize.y);
					child->mElement->_setAreaDepth(areaDepth);
					child->mElement->_setWidgetDepth(widgetDepth);

					RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
					child->mElement->_setClipRect(elemClipRect);

					offset.y += elementSize.y + ELEMENT_EXTRA_SPACING;
				}

				// TODO - Position expand buttons
				
				todo.push(UpdateTreeElement(child, indent + 1));
			}
		}
	}

	const String& GUISceneTreeView::getGUITypeName()
	{
		static String typeName = "SceneTreeView";
		return typeName;
	}
}