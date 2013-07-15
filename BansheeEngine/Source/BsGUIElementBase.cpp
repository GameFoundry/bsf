#include "BsGUIElementBase.h"
#include "BsGUILayout.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIElementBase::GUIElementBase()
		:mIsDirty(true), mParentLayout(nullptr)
	{

	}

	bool GUIElementBase::_isContentDirty() const
	{
		if((mIsDirty & 0x01) != 0)
			return true;

		for(auto& child : mChildren)
		{
			if(child->_getType() == Type::Layout)
			{
				if(child->_isContentDirty())
					return true;
			}
		}

		return false;
	}

	bool GUIElementBase::_isMeshDirty() const
	{
		return (mIsDirty & 0x02) != 0;
	}

	void GUIElementBase::markContentAsDirty() 
	{ 
		if(!_isContentDirty())
		{
			if(mParentLayout != nullptr)
				mParentLayout->markContentAsDirty();

			mIsDirty |= 0x01; 
		}
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
			if(child->_getType() == Type::Layout)
				child->_updateOptimalLayoutSizes();
		}
	}

	void GUIElementBase::_updateLayoutInternal(CM::UINT32 x, CM::UINT32 y, CM::UINT32 width, CM::UINT32 height, CM::UINT8 widgetDepth, CM::UINT16 areaDepth)
	{
		for(auto& child : mChildren)
		{
			if(child->_getType() == Type::Layout)
				child->_updateLayout(x, y, width, height, widgetDepth, areaDepth);
		}
	}
}