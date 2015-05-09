#include "BsGUILayoutUtility.h"
#include "BsGUIElementBase.h"
#include "BsGUILayout.h"
#include "BsGUIElement.h"
#include "BsGUIElementStyle.h"
#include "BsGUIWidget.h"
#include "BsViewport.h"
#include "BsGUIPanel.h"

namespace BansheeEngine
{
	Vector2I GUILayoutUtility::calcOptimalSize(const GUIElementBase* elem)
	{
		return elem->_calculateLayoutSizeRange().optimal;
	}

	Vector2I GUILayoutUtility::calcActualSize(UINT32 width, UINT32 height, const GUILayout* layout)
	{
		UINT32 numElements = (UINT32)layout->_getNumChildren();

		Vector<LayoutSizeRange> sizeRanges;
		for (UINT32 i = 0; i < numElements; i++)
		{
			GUIElementBase* child = layout->_getChild(i);
			sizeRanges.push_back(child->_calculateLayoutSizeRange());
		}

		Rect2I* elementAreas = nullptr;

		if (numElements > 0)
			elementAreas = stackConstructN<Rect2I>(numElements);

		Rect2I parentArea;
		parentArea.width = width;
		parentArea.height = height;

		layout->_getElementAreas(parentArea, elementAreas, numElements, sizeRanges, layout->_calculateLayoutSizeRange());
		Rect2I* actualAreas = elementAreas; // We re-use the same array

		for (UINT32 i = 0; i < numElements; i++)
		{
			GUIElementBase* child = layout->_getChild(i);
			Rect2I childArea = elementAreas[i];

			if (child->_getType() == GUIElementBase::Type::Layout || child->_getType() == GUIElementBase::Type::Panel)
			{
				Vector2I childActualSize = calcActualSize(childArea.width, childArea.height, static_cast<GUILayout*>(child));
				actualAreas[i].width = (UINT32)childActualSize.x;
				actualAreas[i].height = (UINT32)childActualSize.y;
			}
			else if (child->_getType() == GUIElementBase::Type::Element)
			{
				RectOffset padding = child->_getPadding();

				actualAreas[i].width = elementAreas[i].width + padding.left + padding.right;
				actualAreas[i].height = elementAreas[i].height + padding.top + padding.bottom;
			}
			else
			{
				actualAreas[i].width = elementAreas[i].width;
				actualAreas[i].height = elementAreas[i].height;
			}
		}

		Vector2I actualSize = layout->_calcActualSize(0, 0, actualAreas, numElements);

		if (elementAreas != nullptr)
			stackDeallocLast(elementAreas);

		return actualSize;
	}
}