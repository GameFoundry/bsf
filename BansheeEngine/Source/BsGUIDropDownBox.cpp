#include "BsGUIDropDownBox.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUITexture.h"
#include "BsGUIButton.h"
#include "BsGUIContent.h"
#include "BsGUISkin.h"
#include "CmViewport.h"
#include "BsGUIDropDownList.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const UINT32 GUIDropDownBox::DROP_DOWN_BOX_WIDTH = 150;

	GUIDropDownData GUIDropDownData::separator()
	{
		GUIDropDownData data;
		data.mType = Type::Separator;
		data.mCallback = nullptr;

		return data;
	}

	GUIDropDownData GUIDropDownData::button(const CM::WString& label, std::function<void()> callback, bool isExpandable)
	{
		GUIDropDownData data;
		data.mLabel = label;
		data.mType = isExpandable ? Type::EntryExpandable : Type::Entry;
		data.mCallback = callback;

		return data;
	}

	GUIDropDownBox::GUIDropDownBox(const HSceneObject& parent)
		:GUIWidget(parent), mDropDownStartIdx(0), mBackgroundFrame(nullptr), mScrollUpStyle(nullptr),
		mScrollDownStyle(nullptr), mEntryBtnStyle(nullptr), mEntryExpBtnStyle(nullptr), 
		mSeparatorStyle(nullptr), mBackgroundStyle(nullptr), mBackgroundArea(nullptr), mContentArea(nullptr), 
		mContentLayout(nullptr), mScrollUpBtn(nullptr), mScrollDownBtn(nullptr)
	{

	}

	GUIDropDownBox::~GUIDropDownBox()
	{

	}

	void GUIDropDownBox::initialize(Viewport* target, RenderWindow* window, GUIElement* parentElem, 
		const CM::Vector<GUIDropDownData>::type& elements, const GUISkin& skin)
	{
		GUIWidget::initialize(target, window);

		mElements = elements;

		mScrollUpStyle = skin.getStyle("DropDownScrollUpBtn");
		mScrollDownStyle = skin.getStyle("DropDownScrollDownBtn");
		mEntryBtnStyle = skin.getStyle("DropDownEntryBtn");
		mEntryExpBtnStyle = skin.getStyle("DropDownEntryExpBtn");
		mSeparatorStyle = skin.getStyle("DropDownSeparator");
		mBackgroundStyle = skin.getStyle("DropDownBox");

		mScrollUpBtnArrow = skin.getStyle("DropDownScrollUpBtnArrow")->normal.texture;
		mScrollDownBtnArrow = skin.getStyle("DropDownScrollDownBtnArrow")->normal.texture;

		setDepth(0); // Needs to be in front of everything

		Rect dropDownListBounds = parentElem->getBounds();

		Int2 position;
		// Determine x position and whether to align to left or right side of the drop down list
		UINT32 availableRightwardWidth = (UINT32)std::max(0, (target->getLeft() + target->getWidth()) - dropDownListBounds.x);
		UINT32 availableLeftwardWidth = (UINT32)std::max(0, (dropDownListBounds.x + dropDownListBounds.width) - target->getLeft());

		//// Prefer right if possible
		if(DROP_DOWN_BOX_WIDTH <= availableRightwardWidth)
			position.x = dropDownListBounds.x;
		else
		{
			if(availableRightwardWidth >= availableLeftwardWidth)
				position.x = dropDownListBounds.x;
			else
				position.x = dropDownListBounds.x - std::min(DROP_DOWN_BOX_WIDTH, availableLeftwardWidth);
		}

		// Determine maximum width
		UINT32 maxPossibleWidth = (UINT32)std::max(0, (target->getLeft() + target->getWidth()) - position.x);
		UINT32 width = std::min(DROP_DOWN_BOX_WIDTH, maxPossibleWidth);

		// Determine y position and whether to open upward or downward
		UINT32 helperElementHeight = mScrollUpStyle->height + mScrollDownStyle->height + mBackgroundStyle->margins.top + mBackgroundStyle->margins.bottom;

		UINT32 maxNeededHeight = helperElementHeight;
		UINT32 numElements = (UINT32)mElements.size();
		for(UINT32 i = 0; i < numElements; i++)
			maxNeededHeight += getElementHeight(i);

		UINT32 availableDownwardHeight = (UINT32)std::max(0, (target->getTop() + target->getHeight()) - (dropDownListBounds.y + dropDownListBounds.height));
		UINT32 availableUpwardHeight = (UINT32)std::max(0, dropDownListBounds.y - target->getTop());

		//// Prefer down if possible
		UINT32 height = 0;
		if(maxNeededHeight <= availableDownwardHeight)
		{
			position.y = dropDownListBounds.y + dropDownListBounds.height;
			height = availableDownwardHeight;
		}
		else
		{
			if(availableDownwardHeight >= availableUpwardHeight)
			{
				position.y = dropDownListBounds.y + dropDownListBounds.height;
				height = availableDownwardHeight;
			}
			else
			{
				position.y = dropDownListBounds.y - std::min(maxNeededHeight, availableUpwardHeight);
				height = availableUpwardHeight;
			}
		}

		// Content area
		mContentArea = GUIArea::create(*this, position.x, position.y, width, height);
		mContentLayout = &mContentArea->getLayout().addLayoutY();

		// Background frame
		mBackgroundArea = GUIArea::create(*this, position.x, position.y, width, height);
		mBackgroundArea->setDepth(102);
		mBackgroundArea->getLayout().addElement(GUITexture::create(*this, GUIImageScaleMode::ScaleToFit, skin.getStyle("DropDownBox")));

		updateGUIElements(position.x, position.y, width, height);
	}

	void GUIDropDownBox::updateGUIElements(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height)
	{
		// Remove all elements from content layout
		while(mContentLayout->getNumChildren() > 0)
			mContentLayout->removeChildAt(mContentLayout->getNumChildren() - 1);

		// Determine if we need scroll up and/or down buttons, number of visible elements and actual height
		bool needsScrollUp = mDropDownStartIdx > 0;
		UINT32 numElements = (UINT32)mElements.size();

		UINT32 usedHeight = mBackgroundStyle->margins.top + mBackgroundStyle->margins.bottom;
		if(needsScrollUp)
			usedHeight += mScrollUpStyle->height;

		UINT32 numVisibleElements = 0;
		bool needsScrollDown = false;
		for(UINT32 i = 0; i < numElements; i++)
		{
			usedHeight += getElementHeight(i);
			numVisibleElements++;

			if(usedHeight > height)
			{
				usedHeight += mScrollDownStyle->height;
				needsScrollDown = true;

				// Remove last few elements until we fit again
				UINT32 j = i;
				while(usedHeight > height && j >= 0)
				{
					usedHeight -= getElementHeight(j);
					numVisibleElements--;

					j--;
				}

				break;
			}
		}

		// Add scroll up button
		if(needsScrollUp)
		{
			if(mScrollUpBtn == nullptr)
			{
				mScrollUpBtn = GUIButton::create(*this, GUIContent(L"", mScrollUpBtnArrow), mScrollUpStyle);
				mScrollUpBtn->onClick.connect(boost::bind(&GUIDropDownBox::scrollUp, this));
			}

			mContentLayout->addElement(mScrollUpBtn);			
		}
		else
		{
			if(mScrollUpBtn != nullptr)
			{
				GUIElement::destroy(mScrollUpBtn);
				mScrollUpBtn = nullptr;
			}
		}

		CM::Vector<GUITexture*>::type newSeparators;
		CM::Vector<GUIButton*>::type newEntryBtns;
		CM::Vector<GUIButton*>::type newExpEntryBtns;
		for(UINT32 i = 0; i < numVisibleElements; i++)
		{
			GUIDropDownData& element = mElements[i];

			if(element.isSeparator())
			{
				GUITexture* separator = nullptr;
				if(!mCachedSeparators.empty())
				{
					separator = mCachedSeparators.back();
					mCachedSeparators.erase(mCachedSeparators.end() - 1);
				}
				else
				{
					separator = GUITexture::create(*this, GUIImageScaleMode::StretchToFit, mSeparatorStyle);
				}

				mContentLayout->addElement(separator);
				newSeparators.push_back(separator);
			}
			else if(element.isExpandable())
			{
				GUIButton* expEntryBtn = nullptr;
				if(!mCachedExpEntryBtns.empty())
				{
					expEntryBtn = mCachedExpEntryBtns.back();
					mCachedExpEntryBtns.erase(mCachedExpEntryBtns.end() - 1);
				}
				else
				{
					expEntryBtn = GUIButton::create(*this, mElements[i].getLabel(), mEntryExpBtnStyle);
					expEntryBtn->onClick.connect(mElements[i].getCallback());
				}

				mContentLayout->addElement(expEntryBtn);
				newExpEntryBtns.push_back(expEntryBtn);
			}
			else
			{
				GUIButton* entryBtn = nullptr;
				if(!mCachedEntryBtns.empty())
				{
					entryBtn = mCachedEntryBtns.back();
					mCachedEntryBtns.erase(mCachedEntryBtns.end() - 1);
				}
				else
				{
					entryBtn = GUIButton::create(*this, mElements[i].getLabel(), mEntryBtnStyle);
					entryBtn->onClick.connect(mElements[i].getCallback());
				}

				mContentLayout->addElement(entryBtn);
				newEntryBtns.push_back(entryBtn);
			}
		}

		// Destroy all unused cached elements
		for(auto& elem : mCachedSeparators)
			GUIElement::destroy(elem);

		for(auto& elem : mCachedEntryBtns)
			GUIElement::destroy(elem);

		for(auto& elem : mCachedExpEntryBtns)
			GUIElement::destroy(elem);

		mCachedSeparators = newSeparators;
		mCachedEntryBtns = newEntryBtns;
		mCachedExpEntryBtns = newExpEntryBtns;

		// Add scroll down button
		if(needsScrollDown)
		{
			if(mScrollDownBtn == nullptr)
			{
				mScrollDownBtn = GUIButton::create(*this, GUIContent(L"", mScrollDownBtnArrow), mScrollDownStyle);
				mScrollDownBtn->onClick.connect(boost::bind(&GUIDropDownBox::scrollDown, this));
			}

			mContentLayout->addElement(mScrollDownBtn);			
		}
		else
		{
			if(mScrollDownBtn != nullptr)
			{
				GUIElement::destroy(mScrollDownBtn);
				mScrollDownBtn = nullptr;
			}
		}
		
		// Resize and reposition areas
		mBackgroundArea->setSize(width, usedHeight);
		mBackgroundArea->setPosition(x, y);

		UINT32 contentWidth = (UINT32)std::max(0, (INT32)width - (INT32)mBackgroundStyle->margins.left - (INT32)mBackgroundStyle->margins.right);
		UINT32 contentHeight = (UINT32)std::max(0, (INT32)usedHeight - (INT32)mBackgroundStyle->margins.top - (INT32)mBackgroundStyle->margins.bottom);
		mContentArea->setSize(contentWidth, contentHeight);
		mContentArea->setPosition(x + mBackgroundStyle->margins.left, y + mBackgroundStyle->margins.top);
	}

	UINT32 GUIDropDownBox::getElementHeight(CM::UINT32 idx) const
	{
		if(mElements[idx].isSeparator())
			return mSeparatorStyle->height;
		else if(mElements[idx].isExpandable())
			return mEntryExpBtnStyle->height;
		else
			return mEntryBtnStyle->height;
	}

	void GUIDropDownBox::scrollDown()
	{
		//INT32 maxNumElements = (INT32)mDropDownElements.size();
		//INT32 numVisibleElements = (INT32)mDropDownElementButtons.size();

		//INT32 newIdx = mDropDownStartIdx + numVisibleElements;
		//INT32 clampedNewIdx = std::min(newIdx, maxNumElements - numVisibleElements);
		//mDropDownStartIdx = (UINT32)std::min(newIdx, clampedNewIdx);

		//UINT32 i = mDropDownStartIdx;
		//for(auto& button : mDropDownElementButtons)
		//{
		//	button->setContent(GUIContent(mDropDownElements[i]));
		//	i++;
		//}
	}

	void GUIDropDownBox::scrollUp()
	{
		//INT32 numVisibleElements = (INT32)mDropDownElementButtons.size();

		//INT32 newIdx = mDropDownStartIdx - numVisibleElements;
		//INT32 clampedNewIdx = std::max(newIdx, 0);
		//mDropDownStartIdx = (UINT32)std::min(newIdx, clampedNewIdx);

		//UINT32 i = mDropDownStartIdx;
		//for(auto& button : mDropDownElementButtons)
		//{
		//	button->setContent(GUIContent(mDropDownElements[i]));
		//	i++;
		//}
	}
}