#include "BsGUILayout.h"
#include "BsGUIElement.h"
#include "BsGUILayoutX.h"
#include "BsGUILayoutY.h"
#include "BsGUISpace.h"
#include "BsException.h"

namespace BansheeEngine
{
	GUILayout::GUILayout(const GUIDimensions& dimensions)
		:GUIElementBase(dimensions)
	{ }

	GUILayout::GUILayout()
	{ }

	GUILayout::~GUILayout() 
	{ 
		if (mParentElement != nullptr)
			mParentElement->_unregisterChildElement(this);
	}

	void GUILayout::addElement(GUIElementBase* element)
	{
		if (!element->_isDestroyed())
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

		if (element->_isDestroyed())
			return;

		GUIElementBase* parentElement = element->_getParent();
		if(parentElement != nullptr)
		{
			parentElement->_unregisterChildElement(element);
		}

		element->_setParent(this);
		mChildren.insert(mChildren.begin() + idx, element);
		
		if (!mIsVisible)
			element->setVisible(false);

		_markLayoutAsDirty();
	}

	void GUILayout::removeElementAt(UINT32 idx)
	{
		if(idx < 0 || idx >= (UINT32)mChildren.size())
			BS_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUIElementBase* child = mChildren[idx];
		mChildren.erase(mChildren.begin() + idx);

		child->_setParent(nullptr);

		_markLayoutAsDirty();
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