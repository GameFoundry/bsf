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

	Rect2I GUILayoutUtility::calcBounds(const GUIElementBase* elem)
	{
		Rect2I parentArea;

		GUIElementBase* parent = elem->_getParent();
		if (parent != nullptr)
			parentArea = calcBounds(parent);
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

		Rect2I* elementAreas = nullptr;

		if (numElements > 0)
			elementAreas = stackConstructN<Rect2I>(numElements);

		parent->_getElementAreas(parentArea.x, parentArea.y, parentArea.width, parentArea.height, elementAreas, numElements, optimalSizes);
		Rect2I myArea = elementAreas[myIndex];

		if (elementAreas != nullptr)
			stackDeallocLast(elementAreas);

		return myArea;
	}
}