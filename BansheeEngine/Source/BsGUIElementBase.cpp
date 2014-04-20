#include "BsGUIElementBase.h"
#include "BsGUILayout.h"
#include "BsGUILayoutX.h"
#include "BsGUILayoutY.h"
#include "BsGUIElement.h"
#include "CmException.h"

namespace BansheeEngine
{
	GUIElementBase::GUIElementBase()
		:mIsDirty(true), mParentElement(nullptr), mIsDisabled(false), mParentWidget(nullptr)
	{

	}

	GUIElementBase::~GUIElementBase()
	{
		for(auto& child : mChildren)
		{
			// Non-GUIElement are owned by us
			if(child->_getType() != GUIElementBase::Type::Element)
				cm_delete<PoolAlloc>(child);
			else
			{
				GUIElement* element = static_cast<GUIElement*>(child);
				element->_setParent(nullptr);
			}
		}
	}

	bool GUIElementBase::_isContentDirty() const
	{
		if((mIsDirty & 0x01) != 0)
			return true;

		for(auto& child : mChildren)
		{
			if(child->_isContentDirty())
				return true;
		}

		return false;
	}

	bool GUIElementBase::_isMeshDirty() const
	{
		return (mIsDirty & 0x02) != 0;
	}

	void GUIElementBase::markContentAsDirty() 
	{ 
		if(_isDisabled())
			return;

		mIsDirty |= 0x01; 
	}

	void GUIElementBase::markMeshAsDirty()
	{
		if(_isDisabled())
			return;

		mIsDirty |= 0x02;
	}

	void GUIElementBase::enableRecursively()
	{
		// Make sure to mark everything as dirty, as we didn't track any dirty flags while the element was disabled
		mIsDisabled = false;
		markContentAsDirty();

		for(auto& elem : mChildren)
		{
			elem->enableRecursively();
		}
	}

	void GUIElementBase::disableRecursively()
	{
		markMeshAsDirty(); // Just need to hide the mesh
		mIsDisabled = true;

		for(auto& elem : mChildren)
		{
			elem->disableRecursively();
		}
	}

	void GUIElementBase::_updateLayout(INT32 x, INT32 y, UINT32 width, UINT32 height, RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		_updateOptimalLayoutSizes(); // We calculate optimal sizes of all layouts as a pre-processing step, as they are requested often during update
		_updateLayoutInternal(x, y, width, height, clipRect, widgetDepth, areaDepth);
	}

	void GUIElementBase::_updateOptimalLayoutSizes()
	{
		for(auto& child : mChildren)
		{
			child->_updateOptimalLayoutSizes();
		}
	}

	void GUIElementBase::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height, RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		for(auto& child : mChildren)
		{
			child->_updateLayoutInternal(x, y, width, height, clipRect, widgetDepth, areaDepth);
		}
	}

	void GUIElementBase::_setParent(GUIElementBase* parent) 
	{ 
		if(mParentElement != parent)
		{
			mParentElement = parent; 

			if(parent != nullptr)
			{
				if(_getParentWidget() != parent->_getParentWidget())
					_changeParentWidget(parent->_getParentWidget());
			}
		}
	}

	GUILayout& GUIElementBase::addLayoutXInternal(GUIElementBase* parent)
	{
		GUILayoutX* entry = cm_new<GUILayoutX, PoolAlloc>();
		entry->_setParent(parent);

		mChildren.push_back(entry);
		markContentAsDirty();

		return *entry;
	}

	GUILayout& GUIElementBase::addLayoutYInternal(GUIElementBase* parent)
	{
		GUILayoutY* entry = cm_new<GUILayoutY, PoolAlloc>();
		entry->_setParent(parent);

		mChildren.push_back(entry);
		markContentAsDirty();

		return *entry;
	}

	void GUIElementBase::removeLayoutInternal(GUILayout& layout)
	{
		bool foundElem = false;
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			GUIElementBase* child = *iter;

			if(child->_getType() == GUIElementBase::Type::Layout && child == &layout)
			{
				cm_delete<PoolAlloc>(child);

				mChildren.erase(iter);
				foundElem = true;
				markContentAsDirty();
				break;
			}
		}

		if(!foundElem)
			CM_EXCEPT(InvalidParametersException, "Provided element is not a part of this layout.");
	}

	GUILayout& GUIElementBase::insertLayoutXInternal(GUIElementBase* parent, UINT32 idx)
	{
		if(idx < 0 || idx > (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUILayoutX* entry = cm_new<GUILayoutX, PoolAlloc>();
		entry->_setParent(parent);

		mChildren.insert(mChildren.begin() + idx, entry);
		markContentAsDirty();

		return *entry;
	}

	GUILayout& GUIElementBase::insertLayoutYInternal(GUIElementBase* parent, UINT32 idx)
	{
		if(idx < 0 || idx > (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUILayoutY* entry = cm_new<GUILayoutY, PoolAlloc>();
		entry->_setParent(parent);

		mChildren.insert(mChildren.begin() + idx, entry);
		markContentAsDirty();

		return *entry;
	}

	void GUIElementBase::_registerChildElement(GUIElement* element)
	{
		GUIElementBase* parentElement = element->_getParent();
		if(parentElement != nullptr)
		{
			parentElement->_unregisterChildElement(element);
		}

		element->_setParent(this);
		mChildren.push_back(element);

		markContentAsDirty();
	}

	void GUIElementBase::_unregisterChildElement(GUIElement* element)
	{
		bool foundElem = false;
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			GUIElementBase* child = *iter;

			if(child->_getType() == GUIElementBase::Type::Element && child == element)
			{
				mChildren.erase(iter);
				element->_setParent(nullptr);
				foundElem = true;

				markContentAsDirty();
				break;
			}
		}

		if(!foundElem)
			CM_EXCEPT(InvalidParametersException, "Provided element is not a part of this element.");
	}

	void GUIElementBase::_changeParentWidget(GUIWidget* widget)
	{
		mParentWidget = widget;

		for(auto& child : mChildren)
		{
			child->_changeParentWidget(widget);
		}

		markContentAsDirty();
	}
}