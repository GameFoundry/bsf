#include "BsGUIManager.h"
#include "BsGUIWidget.h"
#include "BsGUIElement.h"
#include "CmMaterial.h"
#include "CmMeshData.h"
#include "CmMesh.h"
#include "CmUtil.h"

namespace BansheeEngine
{
	void GUIManager::registerWidget(GUIWidget* widget)
	{
		mWidgets.push_back(widget);
	}

	void GUIManager::unregisterWidget(GUIWidget* widget)
	{
		auto findIter = std::find(begin(mWidgets), end(mWidgets), widget);

		if(findIter != end(mWidgets))
			mWidgets.erase(findIter);
	}
}