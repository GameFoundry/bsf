#include "BsGUIElementBase.h"
#include "BsGUILayout.h"
#include "BsGUILayoutX.h"
#include "BsGUILayoutY.h"
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIElementBase::GUIElementBase()
		:mIsDirty(true)
	{

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
		mIsDirty |= 0x01; 
	}

	void GUIElementBase::markMeshAsDirty()
	{
		mIsDirty |= 0x02;
	}

	void GUIElementBase::_updateLayout(UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT8 widgetDepth, UINT16 areaDepth)
	{
		_updateOptimalLayoutSizes(); // We calculate optimal sizes of all layouts as a pre-processing step, as they are requested often during update
		_updateLayoutInternal(x, y, width, height, widgetDepth, areaDepth);
	}

	void GUIElementBase::_updateOptimalLayoutSizes()
	{
		for(auto& child : mChildren)
		{
			child->_updateOptimalLayoutSizes();
		}
	}

	void GUIElementBase::_updateLayoutInternal(CM::UINT32 x, CM::UINT32 y, CM::UINT32 width, CM::UINT32 height, CM::UINT8 widgetDepth, CM::UINT16 areaDepth)
	{
		for(auto& child : mChildren)
		{
			child->_updateLayoutInternal(x, y, width, height, widgetDepth, areaDepth);
		}
	}

	GUILayout& GUIElementBase::addLayoutXInternal()
	{
		GUILayoutX* entry = cm_new<GUILayoutX, PoolAlloc>();

		mChildren.push_back(entry);
		markContentAsDirty();

		return *entry;
	}

	GUILayout& GUIElementBase::addLayoutYInternal()
	{
		GUILayoutY* entry = cm_new<GUILayoutY, PoolAlloc>();

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

	GUILayout& GUIElementBase::insertLayoutXInternal(UINT32 idx)
	{
		if(idx < 0 || idx >= (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUILayoutX* entry = cm_new<GUILayoutX, PoolAlloc>();

		mChildren.insert(mChildren.begin() + idx, entry);
		markContentAsDirty();

		return *entry;
	}

	GUILayout& GUIElementBase::insertLayoutYInternal(UINT32 idx)
	{
		if(idx < 0 || idx >= (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUILayoutY* entry = cm_new<GUILayoutY, PoolAlloc>();

		mChildren.insert(mChildren.begin() + idx, entry);
		markContentAsDirty();

		return *entry;
	}
}