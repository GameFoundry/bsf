#include "CmPlatform.h"

namespace CamelotFramework
{
	OSDropTarget::OSDropTarget(const RenderWindow* ownerWindow, INT32 x, INT32 y, UINT32 width, UINT32 height)
		:mOwnerWindow(ownerWindow), mX(x), mY(y), mWidth(width), mHeight(height), mDropType(OSDropType::None), mFileList(nullptr)
	{
		
	}

	OSDropTarget::~OSDropTarget()
	{
		clean();
	}

	void OSDropTarget::setArea(INT32 x, INT32 y, UINT32 width, UINT32 height)
	{
		mX = x;
		mY = y;
		mWidth = width;
		mHeight = height;
	}

	void OSDropTarget::clean()
	{
		if(mFileList != nullptr)
			cm_delete(mFileList);
	}

	void OSDropTarget::setFileList(const Vector<WString>::type& fileList)
	{
		clean();

		mFileList = cm_new<Vector<WString>::type>();
		*mFileList = fileList;
	}
}