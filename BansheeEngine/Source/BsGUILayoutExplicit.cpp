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

	Vector2I GUILayoutExplicit::_calculateOptimalLayoutSize() const
	{
		return Vector2I(0, 0);
	}

	void GUILayoutExplicit::_updateOptimalLayoutSizes()
	{

	}

	void GUILayoutExplicit::_getElementAreas(INT32 x, INT32 y, UINT32 width, UINT32 height, Rect2I* elementAreas, UINT32 numElements, const Vector<Vector2I>& optimalSizes) const
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

		mActualWidth = width;
		mActualHeight = height;
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

		_markAsClean();
	}
}