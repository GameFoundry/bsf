#include "BsGUIPanel.h"
#include "BsGUIElement.h"
#include "BsGUISpace.h"
#include "BsMath.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	GUIPanel::GUIPanel(INT16 depth, UINT16 depthRangeMin, UINT16 depthRangeMax, const GUIDimensions& dimensions)
		: GUILayout(dimensions), mDepthOffset(depth), mDepthRangeMin(depthRangeMin), mDepthRangeMax(depthRangeMax)
	{ }

	void GUIPanel::setDepthRange(INT16 depth, UINT16 depthRangeMin, UINT16 depthRangeMax)
	{
		mDepthOffset = depth;
		mDepthRangeMin = depthRangeMin;
		mDepthRangeMax = depthRangeMax;

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
			const GUIDimensions& dimensions = child->_getDimensions();

			elementAreas[childIdx].x = x + dimensions.x;
			elementAreas[childIdx].y = y + dimensions.y;

			if (dimensions.fixedWidth())
				elementAreas[childIdx].width = (UINT32)sizeRanges[childIdx].optimal.x;
			else
			{
				UINT32 modifiedWidth = (UINT32)std::max(0, (INT32)width - dimensions.x);
				
				if (modifiedWidth > (UINT32)sizeRanges[childIdx].optimal.x)
				{
					if (dimensions.maxWidth > 0)
						modifiedWidth = std::min(modifiedWidth, dimensions.maxWidth);
				}
				else if (modifiedWidth < (UINT32)sizeRanges[childIdx].optimal.x)
				{
					if (dimensions.minWidth > 0)
						modifiedWidth = std::max(modifiedWidth, dimensions.minWidth);
				}

				elementAreas[childIdx].width = modifiedWidth;
			}

			if (dimensions.fixedHeight())
				elementAreas[childIdx].height = (UINT32)sizeRanges[childIdx].optimal.y;
			else
			{
				UINT32 modifiedHeight = (UINT32)std::max(0, (INT32)height - dimensions.y);

				if (modifiedHeight > (UINT32)sizeRanges[childIdx].optimal.y)
				{
					if (dimensions.maxHeight > 0)
						modifiedHeight = std::min(modifiedHeight, dimensions.maxHeight);
				}
				else if (modifiedHeight < (UINT32)sizeRanges[childIdx].optimal.y)
				{
					if (dimensions.minHeight > 0)
						modifiedHeight = std::max(modifiedHeight, dimensions.minHeight);
				}

				elementAreas[childIdx].height = modifiedHeight;
			}

			childIdx++;
		}
	}

	void GUIPanel::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height, Rect2I clipRect, UINT8 widgetDepth, 
		INT16 panelDepth, UINT16 panelDepthRangeMin, UINT16 panelDepthRangeMax)
	{
		INT32 newPanelDepth = panelDepth + mDepthOffset;
		INT32 newPanelDepthRangeMin = newPanelDepth - mDepthRangeMin;
		INT32 newPanelDepthRangeMax = newPanelDepth + mDepthRangeMax;

		INT32* allDepths[3] = { &newPanelDepth, &newPanelDepthRangeMin, &newPanelDepthRangeMax };

		for (auto& depth : allDepths)
		{
			INT32 minValue = std::max((INT32)panelDepth - (INT32)panelDepthRangeMin, (INT32)std::numeric_limits<INT16>::min());
			*depth = std::max(*depth, minValue);

			INT32 maxValue = std::min((INT32)panelDepth + (INT32)panelDepthRangeMax, (INT32)std::numeric_limits<INT16>::max());
			*depth = std::min(*depth, maxValue);
		}

		panelDepth = (INT16)newPanelDepth;
		
		if (mDepthRangeMin != (UINT16)-1 || panelDepthRangeMin != (UINT16)-1)
			panelDepthRangeMin = (UINT16)(newPanelDepth - newPanelDepthRangeMin);

		if (mDepthRangeMax != (UINT16)-1 || panelDepthRangeMax != (UINT16)-1)
			panelDepthRangeMax = (UINT16)(newPanelDepthRangeMax - newPanelDepth);

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
				element->_updateLayoutInternal(offset.x, offset.y, element->_getWidth(), element->_getHeight(), newClipRect, 
					widgetDepth, panelDepth, panelDepthRangeMin, panelDepthRangeMax);

				actualSizes[childIdx].width = childArea.width + child->_getPadding().top + child->_getPadding().bottom;
				actualSizes[childIdx].height = childArea.height + child->_getPadding().top + child->_getPadding().bottom;
			}
			else if (child->_getType() == GUIElementBase::Type::Layout || child->_getType() == GUIElementBase::Type::Panel)
			{
				GUILayout* layout = static_cast<GUILayout*>(child);

				Rect2I newClipRect(childArea.x, childArea.y, childArea.width, childArea.height);
				newClipRect.clip(clipRect);
				layout->_updateLayoutInternal(offset.x, offset.y, childArea.width, childArea.height, clipRect, 
					widgetDepth, panelDepth, panelDepthRangeMin, panelDepthRangeMax);

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

	GUIPanel* GUIPanel::create(INT16 depth, UINT16 depthRangeMin, UINT16 depthRangeMax)
	{
		return bs_new<GUIPanel>(depth, depthRangeMin, depthRangeMax, GUIDimensions::create());
	}

	GUIPanel* GUIPanel::create(const GUIOptions& options)
	{
		return bs_new<GUIPanel>(0, -1, -1, GUIDimensions::create(options));
	}

	GUIPanel* GUIPanel::create(INT16 depth, UINT16 depthRangeMin, UINT16 depthRangeMax, const GUIOptions& options)
	{
		return bs_new<GUIPanel>(depth, depthRangeMin, depthRangeMax, GUIDimensions::create(options));
	}
}