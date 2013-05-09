#include "BsGUIManager.h"
#include "BsGUIWidget.h"
#include "BsGUIElement.h"
#include "CmMaterial.h"
#include "CmMeshData.h"
#include "CmMesh.h"
#include "CmUtil.h"
#include "CmRenderWindowManager.h"
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIManager::GUIManager()
	{
		//windowCreateConn = RenderWindowManager::instance().onWindowCreated.connect(boost::bind(&GUIManager::doOnWindowCreated, this, _1));
		//windowDestroyConn = RenderWindowManager::instance().onWindowDestroyed.connect(boost::bind(&GUIManager::doOnWindowDestroyed, this, _1));
	}

	GUIManager::~GUIManager()
	{
		//RenderWindowManager::instance().onWindowCreated.disconnect(windowCreateConn);
		//RenderWindowManager::instance().onWindowDestroyed.disconnect(windowDestroyConn);
	}

	void GUIManager::registerWidget(GUIWidget* widget)
	{
		mWidgets.push_back(widget);
	}

	void GUIManager::unregisterWidget(GUIWidget* widget)
	{
		auto findIter = std::find(begin(mWidgets), end(mWidgets), widget);

		if(findIter != end(mWidgets))
			mWidgets.erase(findIter);

		for(auto& windowMap : mInputMap)
		{
			auto& widgets = windowMap.second;
			auto iterFind = std::find(begin(widgets), end(widgets), widget);

			if(iterFind != end(widgets))
				widgets.erase(iterFind);
		}
	}

	void GUIManager::attachWidgetToWindow(const RenderWindow* window, GUIWidget* widget)
	{
		auto findIter = mInputMap.find(window);

		if(findIter == mInputMap.end())
			CM_EXCEPT(InternalErrorException, "Cannot find specified window in the window list.");

		std::vector<GUIWidget*>& widgets = findIter->second;

		bool found = false;
		for(auto& existingWidget : widgets)
		{
			if(existingWidget == widget)
			{
				found = true;
				break;
			}
		}

		if(!found)
			widgets.push_back(widget);
	}

	void GUIManager::update()
	{

	}

	void GUIManager::doOnWindowCreated(RenderWindow* window)
	{
		mInputMap.insert(std::make_pair(window, std::vector<GUIWidget*>()));
	}

	void GUIManager::doOnWindowDestroyed(RenderWindow* window)
	{
		auto findIter = mInputMap.find(window);

		if(findIter == mInputMap.end())
			CM_EXCEPT(InternalErrorException, "Cannot find specified window in the window list.");

		mInputMap.erase(findIter);
	}
}