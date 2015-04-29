#include "BsGUILayout.h"
#include "BsGUIElement.h"
#include "BsGUILayoutX.h"
#include "BsGUILayoutY.h"
#include "BsGUISpace.h"
#include "BsException.h"

namespace BansheeEngine
{
	GUILayout::GUILayout(GUIArea* parentArea)
		:mParentGUIArea(parentArea)
	{ }

	GUILayout::GUILayout(const GUIDimensions& dimensions)
		:GUIElementBase(dimensions), mParentGUIArea(nullptr)
	{ }

	GUILayout::GUILayout()
		:mParentGUIArea(nullptr)
	{ }

	GUILayout::~GUILayout() 
	{ 
		if (mParentElement != nullptr)
			mParentElement->_unregisterChildElement(this);
	}

	void GUILayout::addElement(GUIElementBase* element)
	{
		_registerChildElement(element);
	}

	void GUILayout::removeElement(GUIElementBase* element)
	{
		_unregisterChildElement(element);
	}

	void GUILayout::insertElement(UINT32 idx, GUIElementBase* element)
	{
		if(idx < 0 || idx > (UINT32)mChildren.size())
			BS_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUIElementBase* parentElement = element->_getParent();
		if(parentElement != nullptr)
		{
			parentElement->_unregisterChildElement(element);
		}

		element->_setParent(this);
		mChildren.insert(mChildren.begin() + idx, element);
		
		if (mIsDisabled)
			element->disableRecursively();

		markContentAsDirty();
	}

	void GUILayout::removeElementAt(UINT32 idx)
	{
		if(idx < 0 || idx >= (UINT32)mChildren.size())
			BS_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUIElementBase* child = mChildren[idx];
		mChildren.erase(mChildren.begin() + idx);

		child->_setParent(nullptr);

		markContentAsDirty();
	}

	const RectOffset& GUILayout::_getPadding() const
	{
		static RectOffset padding;

		return padding;
	}

	void GUILayout::destroy(GUILayout* layout)
	{
		bs_delete(layout);
	}
}