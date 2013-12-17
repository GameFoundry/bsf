#include "CmPlatform.h"

namespace CamelotFramework
{
	OSDropTarget::OSDropTarget(const RenderWindow* ownerWindow, INT32 x, INT32 y, UINT32 width, UINT32 height)
		:mOwnerWindow(ownerWindow), mX(x), mY(y), mWidth(width), mHeight(height), mDropType(OSDropType::None), 
		mFileList(nullptr), mActive(false)
	{
		
	}

	OSDropTarget::~OSDropTarget()
	{
		_clear();
	}

	void OSDropTarget::setArea(INT32 x, INT32 y, UINT32 width, UINT32 height)
	{
		mX = x;
		mY = y;
		mWidth = width;
		mHeight = height;
	}

	void OSDropTarget::_clear()
	{
		if(mFileList != nullptr)
		{
			cm_delete(mFileList);
			mFileList = nullptr;
		}
	}

	bool OSDropTarget::_isInside(const Vector2I& pos) const
	{
		INT32 right = mX + mWidth;
		INT32 bottom = mY + mHeight;

		return (pos.x >= mX && pos.x < right && pos.y >= mY && pos.y < bottom);
	}

	void OSDropTarget::_setFileList(const Vector<WString>::type& fileList)
	{
		_clear();

		mFileList = cm_new<Vector<WString>::type>();
		*mFileList = fileList;
	}
}