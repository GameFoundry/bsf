#include "BsGUILayoutExplicit.h"
#include "BsGUIElement.h"
#include "BsGUISpace.h"
#include "BsMath.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	GUILayoutExplicit::GUILayoutExplicit(GUIArea* parentArea)
		:GUILayout(parentArea)
	{ }

	LayoutSizeRange GUILayoutExplicit::_calculateLayoutSizeRange() const
	{
		return LayoutSizeRange();
	}

	void GUILayoutExplicit::_updateOptimalLayoutSizes()
	{

	}

	void GUILayoutExplicit::_getElementAreas(INT32 x, INT32 y, UINT32 width, UINT32 height, Rect2I* elementAreas, UINT32 numElements,
		const Vector<LayoutSizeRange>& sizeRanges, const LayoutSizeRange& mySizeRange) const
	{
		assert(mChildren.size() == numElements);

		// Compute offsets and height
		UINT32 childIdx = 0;
		for (auto& child : mChildren)
		{
			if (child->_getType() == GUIElementBase::Type::Element)
			{
				GUIElement* element = static_cast<GUIElement*>(child);

				elementAreas[childIdx].x = element->_getOffset().x;
				elementAreas[childIdx].y = element->_getOffset().y;
				elementAreas[childIdx].width = element->_getWidth();
				elementAreas[childIdx].height = element->_getHeight();
			}
			else if (child->_getType() == GUIElementBase::Type::Layout)
			{
				GUILayout* layout = static_cast<GUILayout*>(child);

				elementAreas[childIdx].height = height;
				elementAreas[childIdx].width = width;
				elementAreas[childIdx].x = x;
				elementAreas[childIdx].y = y;
			}

			childIdx++;
		}
	}

	void GUILayoutExplicit::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height, Rect2I clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		UINT32 childIdx = 0;

		for (auto& child : mChildren)
		{
			if (child->_getType() == GUIElementBase::Type::Element)
			{
				GUIElement* element = static_cast<GUIElement*>(child);
				element->_setWidgetDepth(widgetDepth);
				element->_setAreaDepth(areaDepth);

				Vector2I offset = element->_getOffset();
				Rect2I elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
				element->_setClipRect(elemClipRect);

				Rect2I newClipRect(offset.x, offset.y, element->_getWidth(), element->_getHeight());
				newClipRect.clip(clipRect);
				element->_updateLayoutInternal(offset.x, offset.y, element->_getWidth(), element->_getHeight(), newClipRect, widgetDepth, areaDepth);
			}
			else if (child->_getType() == GUIElementBase::Type::Layout)
			{
				GUILayout* layout = static_cast<GUILayout*>(child);

				layout->_updateLayoutInternal(x, y, width, height, clipRect, widgetDepth, areaDepth);
			}

			childIdx++;
		}

		mActualWidth = 0; // Not relevant
		mActualHeight = 0;

		_markAsClean();
	}

	Vector2I GUILayoutExplicit::_calcActualSize(Rect2I* elementAreas, UINT32 numElements) const
	{
		return Vector2I(0, 0); // Not relevant
	}

	GUILayoutExplicit* GUILayoutExplicit::create()
	{
		return bs_new<GUILayoutExplicit>();
	}
}