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

	GUIDropDownBox::GUIDropDownBox(const HSceneObject& parent)
		:GUIWidget(parent), mDropDownStartIdx(0)
	{

	}

	GUIDropDownBox::~GUIDropDownBox()
	{

	}

	void GUIDropDownBox::initialize(Viewport* target, RenderWindow* window, GUIDropDownList* parentList, 
		const CM::Vector<WString>::type& elements, std::function<void(CM::UINT32)> selectedCallback, const GUISkin& skin)
	{
		GUIWidget::initialize(target, window);

		mDropDownElements = elements;
		mSelectedDropDownEntryCallback = selectedCallback;

		setDepth(0); // Needs to be in front of everything

		const GUIElementStyle* dropDownBoxStyle = skin.getStyle("DropDownBox");

		Rect dropDownListBounds = parentList->getBounds();

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
		UINT32 contentWidth = (UINT32)std::max(0, (INT32)width - (INT32)dropDownBoxStyle->margins.left - (INT32)dropDownBoxStyle->margins.right);

		// Determine y position and whether to open upward or downward
		UINT32 scrollButtonUpHeight = skin.getStyle("DropDownScrollUpBtn")->height;
		UINT32 scrollButtonDownHeight = skin.getStyle("DropDownScrollDownBtn")->height;
		UINT32 helperElementHeight = scrollButtonUpHeight + scrollButtonDownHeight + dropDownBoxStyle->margins.top + dropDownBoxStyle->margins.bottom;
		UINT32 elementButtonHeight = skin.getStyle("DropDownEntryBtn")->height;

		UINT32 maxNeededHeight = elementButtonHeight * (UINT32)mDropDownElements.size() + helperElementHeight;
		UINT32 availableDownwardHeight = (UINT32)std::max(0, (target->getTop() + target->getHeight()) - (dropDownListBounds.y + dropDownListBounds.height));

		UINT32 availableUpwardHeight = (UINT32)std::max(0, dropDownListBounds.y - target->getTop());

		//// Prefer down if possible
		if(maxNeededHeight <= availableDownwardHeight)
			position.y = dropDownListBounds.y + dropDownListBounds.height;
		else
		{
			if(availableDownwardHeight >= availableUpwardHeight)
				position.y = dropDownListBounds.y + dropDownListBounds.height;
			else
				position.y = dropDownListBounds.y - std::min(maxNeededHeight, availableUpwardHeight);
		}

		// Determine height of the box, and how many visible elements we can fit in it
		UINT32 maxPossibleHeight = (UINT32)std::max(0, (target->getTop() + target->getHeight()) - position.y);
		UINT32 heightAvailableForContent = (UINT32)std::max(0, (INT32)maxPossibleHeight - (INT32)helperElementHeight);
		UINT32 numVisElements = 0;
		
		UINT32 contentAreaHeight = 0;
		for(UINT32 i = 0; i < (UINT32)mDropDownElements.size(); i++)
		{
			if(contentAreaHeight >= heightAvailableForContent)
				break;

			contentAreaHeight += elementButtonHeight;
			numVisElements++;
		}

		UINT32 totalHeight = contentAreaHeight + helperElementHeight; 
		
		// Scroll up buttons
		GUIArea* scrollUpBtnArea = GUIArea::create(*this, position.x + dropDownBoxStyle->margins.left, position.y, contentWidth, scrollButtonUpHeight);
		scrollUpBtnArea->setDepth(100);
		const GUIElementStyle* scrollUpBtnArrow = skin.getStyle("DropDownScrollUpBtnArrow");
		GUIButton* scrollUpBtn = GUIButton::create(*this, GUIContent(L"", scrollUpBtnArrow->normal.texture), skin.getStyle("DropDownScrollUpBtn"));
		scrollUpBtnArea->getLayout().addElement(scrollUpBtn);

		// Entry buttons
		// TODO - It may happen there is not enough place for even a single element, in which case we just don't render any.
		//  Some smart solution to deal with that case might be employed, but I think it's a fairly rare case and can be deal with in other ways.

		UINT32 contentAreaYOffset = scrollButtonUpHeight + dropDownBoxStyle->margins.top;

		GUIArea* dropDownEntriesArea = GUIArea::create(*this, position.x + dropDownBoxStyle->margins.left, 
			position.y + contentAreaYOffset, contentWidth, contentAreaHeight);
		GUILayout* dropDownEntriesLayout = &dropDownEntriesArea->getLayout().addLayoutY();

		for(UINT32 i = 0; i < numVisElements; i++)
		{
			GUIButton* button = GUIButton::create(*this, mDropDownElements[i], skin.getStyle("DropDownEntryBtn"));
			button->onClick.connect(boost::bind(&GUIDropDownBox::entrySelected, this, i));

			dropDownEntriesLayout->addElement(button);
			mDropDownElementButtons.push_back(button);
		}

		// Scroll down buttons
		UINT32 scrollBtnDownOffset = position.y + contentAreaYOffset + contentAreaHeight;

		GUIArea* scrollDownBtnArea = GUIArea::create(*this, position.x + dropDownBoxStyle->margins.left, 
			scrollBtnDownOffset, contentWidth, scrollButtonDownHeight);
		scrollDownBtnArea->setDepth(100);
		const GUIElementStyle* scrollDownBtnArrow = skin.getStyle("DropDownScrollDownBtnArrow");
		GUIButton* scrollDownBtn = GUIButton::create(*this, GUIContent(L"", scrollDownBtnArrow->normal.texture), skin.getStyle("DropDownScrollDownBtn"));
		scrollDownBtnArea->getLayout().addElement(scrollDownBtn);
		
		// Background frame
		GUIArea* dropDownBoxArea = GUIArea::create(*this, position.x, position.y, width, totalHeight);
		dropDownBoxArea->setDepth(102);
		dropDownBoxArea->getLayout().addElement(GUITexture::create(*this, GUIImageScaleMode::ScaleToFit, skin.getStyle("DropDownBox")));
	}

	void GUIDropDownBox::scrollDown()
	{
		INT32 maxNumElements = (INT32)mDropDownElements.size();
		INT32 numVisibleElements = (INT32)mDropDownElementButtons.size();

		INT32 newIdx = mDropDownStartIdx + numVisibleElements;
		INT32 clampedNewIdx = std::min(newIdx, maxNumElements - numVisibleElements);
		mDropDownStartIdx = (UINT32)std::min(newIdx, clampedNewIdx);

		UINT32 i = mDropDownStartIdx;
		for(auto& button : mDropDownElementButtons)
		{
			button->setContent(GUIContent(mDropDownElements[i]));
			i++;
		}
	}

	void GUIDropDownBox::scrollUp()
	{
		INT32 numVisibleElements = (INT32)mDropDownElementButtons.size();

		INT32 newIdx = mDropDownStartIdx - numVisibleElements;
		INT32 clampedNewIdx = std::max(newIdx, 0);
		mDropDownStartIdx = (UINT32)std::min(newIdx, clampedNewIdx);

		UINT32 i = mDropDownStartIdx;
		for(auto& button : mDropDownElementButtons)
		{
			button->setContent(GUIContent(mDropDownElements[i]));
			i++;
		}
	}

	void GUIDropDownBox::entrySelected(UINT32 idx)
	{
		if(mSelectedDropDownEntryCallback != nullptr)
			mSelectedDropDownEntryCallback(mDropDownStartIdx + idx);
	}
}