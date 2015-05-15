#include "BsDropDownWindowManager.h"
#include "BsDropDownWindow.h"

namespace BansheeEngine
{
	DropDownWindowManager::DropDownWindowManager()
		:mOpenWindow(nullptr)
	{ }

	DropDownWindowManager::~DropDownWindowManager()
	{
		close();
	}

	void DropDownWindowManager::close()
	{
		if (mOpenWindow != nullptr)
		{
			bs_delete(mOpenWindow);
			mOpenWindow = nullptr;
		}
	}

	void DropDownWindowManager::update()
	{
		if (mOpenWindow != nullptr)
			mOpenWindow->update();
	}
}