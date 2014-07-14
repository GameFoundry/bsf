#include "BsGUILayoutUtility.h"
#include "BsGUIElementBase.h"
#include "BsGUILayout.h"
#include "BsGUIArea.h"

namespace BansheeEngine
{
	Vector2I GUILayoutUtility::calcOptimalSize(const GUIElementBase* elem)
	{
		return elem->_calculateOptimalLayoutSize();
	}

	RectI GUILayoutUtility::calcArea(const GUIElementBase* elem)
	{
		RectI parentArea;

		GUIElementBase* parent = elem->_getParent();
		if (parent != nullptr)
			parentArea = calcArea(parent);
		else
		{
			assert(elem->_getType() == GUIElementBase::Type::Layout);
			const GUILayout* layout = static_cast<const GUILayout*>(elem);

			GUIArea* parentGUIArea = layout->_getParentGUIArea();
			parentArea.x = parentGUIArea->x();
			parentArea.y = parentGUIArea->y();
			parentArea.width = parentGUIArea->width();
			parentArea.height = parentGUIArea->height();

			return parentArea;
		}

		UINT32 numElements = (UINT32)parent->_getNumChildren();
		UINT32 myIndex = 0;

		Vector<Vector2I> optimalSizes;
		for (UINT32 i = 0; i < numElements; i++)
		{
			GUIElementBase* child = parent->_getChild(i);

			if (child == elem)
				myIndex = i;

			optimalSizes.push_back(calcOptimalSize(elem));
		}

		RectI* elementAreas = nullptr;

		if (numElements > 0)
			elementAreas = stackConstructN<RectI>(numElements);

		parent->_getElementAreas(parentArea.x, parentArea.y, parentArea.width, parentArea.height, elementAreas, numElements, optimalSizes);
		RectI myArea = elementAreas[myIndex];

		if (elementAreas != nullptr)
			stackDeallocLast(elementAreas);

		return myArea;
	}
}