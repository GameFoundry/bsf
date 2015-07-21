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

		_markContentAsDirty();
	}

	LayoutSizeRange GUIPanel::_calculateLayoutSizeRange() const
	{
		if (mIsDisabled)
			return LayoutSizeRange();

		Vector2I optimalSize;

		for (auto& child : mChildren)
		{
			LayoutSizeRange sizeRange = child->_calculateLayoutSizeRange();

			if (child->_getType() == GUIElementBase::Type::FixedSpace || child->_getType() == GUIElementBase::Type::FlexibleSpace)
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

	LayoutSizeRange GUIPanel::_getElementSizeRange(const GUIElementBase* element) const
	{
		if (element->_getType() == GUIElementBase::Type::FixedSpace || element->_getType() == GUIElementBase::Type::FlexibleSpace)
		{
			LayoutSizeRange sizeRange = element->_calculateLayoutSizeRange();
			sizeRange.optimal.x = 0;
			sizeRange.optimal.y = 0;

			return sizeRange;
		}
		else if (element->_getType() == GUIElementBase::Type::Element)
		{
			return element->_calculateLayoutSizeRange();
		}
		else if (element->_getType() == GUIElementBase::Type::Layout || element->_getType() == GUIElementBase::Type::Panel)
		{
			const GUILayout* layout = static_cast<const GUILayout*>(element);
			return layout->_getCachedSizeRange();
		}

		return LayoutSizeRange();
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
			childSizeRange = _getElementSizeRange(child);

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

	void GUIPanel::_getElementAreas(const Rect2I& layoutArea, Rect2I* elementAreas, UINT32 numElements,
		const Vector<LayoutSizeRange>& sizeRanges, const LayoutSizeRange& mySizeRange) const
	{
		assert(mChildren.size() == numElements);

		// Panel always uses optimal sizes and explicit positions
		UINT32 childIdx = 0;
		for (auto& child : mChildren)
		{
			elementAreas[childIdx] = _getElementArea(layoutArea, child, sizeRanges[childIdx]);

			childIdx++;
		}
	}

	Rect2I GUIPanel::_getElementArea(const Rect2I& layoutArea, const GUIElementBase* element, const LayoutSizeRange& sizeRange) const
	{
		const GUIDimensions& dimensions = element->_getDimensions();

		Rect2I area;

		area.x = layoutArea.x + dimensions.x;
		area.y = layoutArea.y + dimensions.y;

		if (dimensions.fixedWidth())
			area.width = (UINT32)sizeRange.optimal.x;
		else
		{
			UINT32 modifiedWidth = (UINT32)std::max(0, (INT32)layoutArea.width - dimensions.x);

			if (modifiedWidth > (UINT32)sizeRange.optimal.x)
			{
				if (dimensions.maxWidth > 0)
					modifiedWidth = std::min(modifiedWidth, dimensions.maxWidth);
			}
			else if (modifiedWidth < (UINT32)sizeRange.optimal.x)
			{
				if (dimensions.minWidth > 0)
					modifiedWidth = std::max(modifiedWidth, dimensions.minWidth);
			}

			area.width = modifiedWidth;
		}

		if (dimensions.fixedHeight())
			area.height = (UINT32)sizeRange.optimal.y;
		else
		{
			UINT32 modifiedHeight = (UINT32)std::max(0, (INT32)layoutArea.height - dimensions.y);

			if (modifiedHeight > (UINT32)sizeRange.optimal.y)
			{
				if (dimensions.maxHeight > 0)
					modifiedHeight = std::min(modifiedHeight, dimensions.maxHeight);
			}
			else if (modifiedHeight < (UINT32)sizeRange.optimal.y)
			{
				if (dimensions.minHeight > 0)
					modifiedHeight = std::max(modifiedHeight, dimensions.minHeight);
			}

			area.height = modifiedHeight;
		}

		return area;
	}

	void GUIPanel::_updateLayoutInternal(const GUILayoutData& data)
	{
		INT32 newPanelDepth = data.getPanelDepth() + mDepthOffset;
		INT32 newPanelDepthRangeMin = newPanelDepth - mDepthRangeMin;
		INT32 newPanelDepthRangeMax = newPanelDepth + mDepthRangeMax;

		INT32* allDepths[3] = { &newPanelDepth, &newPanelDepthRangeMin, &newPanelDepthRangeMax };

		for (auto& depth : allDepths)
		{
			INT32 minValue = std::max((INT32)data.getPanelDepth() - (INT32)data.depthRangeMin, (INT32)std::numeric_limits<INT16>::min());
			*depth = std::max(*depth, minValue);

			INT32 maxValue = std::min((INT32)data.getPanelDepth() + (INT32)data.depthRangeMax, (INT32)std::numeric_limits<INT16>::max());
			*depth = std::min(*depth, maxValue);
		}

		GUILayoutData childData = data;
		childData.setPanelDepth((INT16)newPanelDepth);
		
		if (mDepthRangeMin != (UINT16)-1 || childData.depthRangeMin != (UINT16)-1)
			childData.depthRangeMin = (UINT16)(newPanelDepth - newPanelDepthRangeMin);

		if (mDepthRangeMax != (UINT16)-1 || childData.depthRangeMax != (UINT16)-1)
			childData.depthRangeMax = (UINT16)(newPanelDepthRangeMax - newPanelDepth);

		UINT32 numElements = (UINT32)mChildren.size();
		Rect2I* elementAreas = nullptr;

		if (numElements > 0)
			elementAreas = bs_stack_new<Rect2I>(numElements);

		_getElementAreas(data.area, elementAreas, numElements, mChildSizeRanges, mSizeRange);

		UINT32 childIdx = 0;

		for (auto& child : mChildren)
		{
			childData.area = elementAreas[childIdx];

			_updateChildLayout(child, childData);

			childIdx++;
		}

		if (elementAreas != nullptr)
			bs_stack_free(elementAreas);
	}

	void GUIPanel::_updateChildLayout(GUIElementBase* element, const GUILayoutData& data)
	{
		GUILayoutData childData = data;

		childData.clipRect = data.area;
		childData.clipRect.clip(data.clipRect);

		element->_setLayoutData(childData);
		element->_updateLayoutInternal(childData);
	}

	Vector2I GUIPanel::_calcActualSize(INT32 x, INT32 y, Rect2I* elementAreas, UINT32 numElements) const
	{
		Vector2I min;
		Vector2I max;

		if (numElements > 0)
		{
			Rect2I childArea = elementAreas[0];

			min = Vector2I(childArea.x, childArea.y);
			max = Vector2I(childArea.x + childArea.width, childArea.y + childArea.height);
		}

		for (UINT32 i = 1; i < numElements; i++)
		{
			Rect2I childArea = elementAreas[i];

			min.x = std::min(min.x, childArea.x);
			min.y = std::min(min.y, childArea.y);

			max.x = std::max(max.x, childArea.x + childArea.width);
			max.y = std::max(max.y, childArea.y + childArea.height);
		}

		return max - min;
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