#include "BsGUIPanel.h"
#include "BsGUIElement.h"
#include "BsGUISpace.h"
#include "BsMath.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	GUIPanel::GUIPanel(UINT16 depth, UINT16 depthRange, const GUIDimensions& dimensions)
		: GUILayout(dimensions), mDepthOffset(depth), mDepthRange(depthRange)
	{ }

	void GUIPanel::setDepthRange(UINT16 depth, UINT16 depthRange)
	{
		mDepthOffset = depth;
		mDepthRange = depthRange;

		markContentAsDirty();
	}

	LayoutSizeRange GUIPanel::_calculateLayoutSizeRange() const
	{
		if (mIsDisabled)
			return LayoutSizeRange();

		Vector2I optimalSize;

		for (auto& child : mChildren)
		{
			LayoutSizeRange sizeRange = child->_calculateLayoutSizeRange();

			if (child->_getType() == GUIElementBase::Type::FixedSpace)
				sizeRange.optimal.x = sizeRange.optimal.y = 0;

			UINT32 paddingX = child->_getPadding().left + child->_getPadding().right;
			UINT32 paddingY = child->_getPadding().top + child->_getPadding().bottom;

			Vector2I childMax(child->_getDimensions().x, child->_getDimensions().y);
			childMax.x += sizeRange.optimal.x + paddingX;
			childMax.y += sizeRange.optimal.y + paddingY;

			optimalSize.x = std::max(optimalSize.x, childMax.x);
			optimalSize.y = std::max(optimalSize.y, childMax.y);
		}

		return _getDimensions().calculateSizeRange(optimalSize);
	}

	void GUIPanel::_updateOptimalLayoutSizes()
	{
		// Update all children first, otherwise we can't determine our own optimal size
		GUIElementBase::_updateOptimalLayoutSizes();

		if (mChildren.size() != mChildSizeRanges.size())
			mChildSizeRanges.resize(mChildren.size());

		Vector2I optimalSize;

		UINT32 childIdx = 0;
		for (auto& child : mChildren)
		{
			LayoutSizeRange& childSizeRange = mChildSizeRanges[childIdx];

			if (child->_getType() == GUIElementBase::Type::FixedSpace)
			{
				GUIFixedSpace* fixedSpace = static_cast<GUIFixedSpace*>(child);

				childSizeRange = fixedSpace->_calculateLayoutSizeRange();
				childSizeRange.optimal.x = 0;
				childSizeRange.optimal.y = 0;
			}
			else if (child->_getType() == GUIElementBase::Type::Element)
			{
				childSizeRange = child->_calculateLayoutSizeRange();
			}
			else if (child->_getType() == GUIElementBase::Type::Layout || child->_getType() == GUIElementBase::Type::Panel)
			{
				GUILayout* layout = static_cast<GUILayout*>(child);
				childSizeRange = layout->_getCachedSizeRange();
			}

			UINT32 paddingX = child->_getPadding().left + child->_getPadding().right;
			UINT32 paddingY = child->_getPadding().top + child->_getPadding().bottom;

			Vector2I childMax(child->_getDimensions().x, child->_getDimensions().y);
			childMax.x += childSizeRange.optimal.x + paddingX;
			childMax.y += childSizeRange.optimal.y + paddingY;

			optimalSize.x = std::max(optimalSize.x, childMax.x);
			optimalSize.y = std::max(optimalSize.y, childMax.y);

			childIdx++;
		}

		mSizeRange = _getDimensions().calculateSizeRange(optimalSize);
	}

	void GUIPanel::_getElementAreas(INT32 x, INT32 y, UINT32 width, UINT32 height, Rect2I* elementAreas, UINT32 numElements,
		const Vector<LayoutSizeRange>& sizeRanges, const LayoutSizeRange& mySizeRange) const
	{
		assert(mChildren.size() == numElements);

		// Panel always uses optimal sizes and explicit positions
		UINT32 childIdx = 0;
		for (auto& child : mChildren)
		{
			elementAreas[childIdx].x = x + child->_getDimensions().x;
			elementAreas[childIdx].y = y + child->_getDimensions().y;
			elementAreas[childIdx].width = (UINT32)sizeRanges[childIdx].optimal.x;
			elementAreas[childIdx].height = (UINT32)sizeRanges[childIdx].optimal.y;

			childIdx++;
		}
	}

	void GUIPanel::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height, Rect2I clipRect, UINT8 widgetDepth, UINT16 panelDepth, UINT16 panelDepthRange)
	{
		UINT32 newPanelDepth = panelDepth + mDepthOffset;
		UINT32 maxPanelDepth = panelDepth + panelDepthRange;

		newPanelDepth = std::min(newPanelDepth, (UINT32)std::numeric_limits<UINT16>::max());

		if (panelDepthRange != (UINT16)-1)
		{
			newPanelDepth = std::min(newPanelDepth, maxPanelDepth);

			UINT16 newRange = (UINT16)(maxPanelDepth - newPanelDepth);

			if (mDepthRange != (UINT16)-1)
				panelDepthRange = std::min(newRange, mDepthRange);
			else
				panelDepthRange = newRange;
		}
		else
			panelDepthRange = mDepthRange;

		panelDepth = (UINT16)newPanelDepth;

		UINT32 numElements = (UINT32)mChildren.size();
		Rect2I* elementAreas = nullptr;

		if (numElements > 0)
			elementAreas = stackConstructN<Rect2I>(numElements);

		_getElementAreas(x, y, width, height, elementAreas, numElements, mChildSizeRanges, mSizeRange);

		UINT32 childIdx = 0;
		Rect2I* actualSizes = elementAreas; // We re-use the same array

		for (auto& child : mChildren)
		{
			Rect2I childArea = elementAreas[childIdx];

			Vector2I offset(childArea.x, childArea.y);
			if (child->_getType() == GUIElementBase::Type::Element)
			{
				GUIElement* element = static_cast<GUIElement*>(child);
				element->_setPosition(offset);
				element->_setWidth(childArea.width);
				element->_setHeight(childArea.height);
				element->_setWidgetDepth(widgetDepth);
				element->_setAreaDepth(panelDepth);

				Vector2I offset = element->_getOffset();
				Rect2I elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
				element->_setClipRect(elemClipRect);

				Rect2I newClipRect(offset.x, offset.y, element->_getWidth(), element->_getHeight());
				newClipRect.clip(clipRect);
				element->_updateLayoutInternal(offset.x, offset.y, element->_getWidth(), element->_getHeight(), newClipRect, widgetDepth, panelDepth, panelDepthRange);

				actualSizes[childIdx].width = childArea.width + child->_getPadding().top + child->_getPadding().bottom;
				actualSizes[childIdx].height = childArea.height + child->_getPadding().top + child->_getPadding().bottom;
			}
			else if (child->_getType() == GUIElementBase::Type::Layout || child->_getType() == GUIElementBase::Type::Panel)
			{
				GUILayout* layout = static_cast<GUILayout*>(child);

				Rect2I newClipRect(childArea.x, childArea.y, childArea.width, childArea.height);
				newClipRect.clip(clipRect);
				layout->_updateLayoutInternal(offset.x, offset.y, childArea.width, childArea.height, clipRect, widgetDepth, panelDepth, panelDepthRange);

				actualSizes[childIdx].width = layout->_getActualWidth();
				actualSizes[childIdx].height = layout->_getActualHeight();
			}
			else
			{
				actualSizes[childIdx].width = childArea.width;
				actualSizes[childIdx].height = childArea.height;
			}

			childIdx++;
		}

		Vector2I actualSize = _calcActualSize(x, y, actualSizes, numElements);
		mActualWidth = (UINT32)actualSize.x;
		mActualHeight = (UINT32)actualSize.y;

		if (elementAreas != nullptr)
			stackDeallocLast(elementAreas);

		_markAsClean();
	}

	Vector2I GUIPanel::_calcActualSize(INT32 x, INT32 y, Rect2I* elementAreas, UINT32 numElements) const
	{
		Vector2I actualArea;
		for (UINT32 i = 0; i < numElements; i++)
		{
			Rect2I childArea = elementAreas[i];

			INT32 diffX = (childArea.x - childArea.width) - x;
			INT32 diffY = (childArea.y - childArea.height) - y;

			actualArea.x = std::max(actualArea.x, diffX);
			actualArea.y = std::max(actualArea.y, diffY);
		}

		return actualArea;
	}

	GUIPanel* GUIPanel::create(UINT16 depth, UINT16 depthRange)
	{
		return bs_new<GUIPanel>(depth, depthRange, GUIDimensions::create());
	}

	GUIPanel* GUIPanel::create(const GUIOptions& options)
	{
		return bs_new<GUIPanel>(0, -1, GUIDimensions::create(options));
	}

	GUIPanel* GUIPanel::create(UINT16 depth, UINT16 depthRange, const GUIOptions& options)
	{
		return bs_new<GUIPanel>(depth, depthRange, GUIDimensions::create(options));
	}
}