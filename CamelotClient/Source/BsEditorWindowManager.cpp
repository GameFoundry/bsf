#include "BsEditorWindowManager.h"
#include "BsEditorWindow.h"
#include "BsMainEditorWindow.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	EditorWindowManager::EditorWindowManager()
		:mMainWindow(nullptr)
	{

	}

	EditorWindowManager::~EditorWindowManager()
	{
		while(mEditorWindows.size() > 0)
			destroy(mEditorWindows[0]);

		if(mMainWindow != nullptr)
			cm_delete(mMainWindow);
	}

	MainEditorWindow* EditorWindowManager::createMain(const CM::RenderWindowPtr& parentRenderWindow)
	{
		if(mMainWindow == nullptr)
			mMainWindow = new (cm_alloc<MainEditorWindow>()) MainEditorWindow(parentRenderWindow);

		return mMainWindow;
	}

	EditorWindow* EditorWindowManager::create()
	{
		EditorWindow* newWindow = new (cm_alloc<EditorWindow>()) EditorWindow();
		mEditorWindows.push_back(newWindow);

		newWindow->initialize();
		return newWindow;
	}

	void EditorWindowManager::destroy(EditorWindowBase* window)
	{
		auto iterFind = std::find(begin(mEditorWindows), end(mEditorWindows), window);

		if(iterFind == end(mEditorWindows))
			CM_EXCEPT(InternalErrorException, "Trying to destroy an editor window that's not registered in the window manager.");

		auto iterFind2 = std::find(begin(mScheduledForDestruction), end(mScheduledForDestruction), window);
		
		if(iterFind2 == end(mScheduledForDestruction))
			mScheduledForDestruction.push_back(window);

		mEditorWindows.erase(iterFind);
	}

	void EditorWindowManager::update()
	{
		// Editor window destroy is deferred to this point, otherwise we risk
		// destroying a window while it's still being used (situation that was happening with GUIManager)
		
		for(auto& windowToDestroy : mScheduledForDestruction)
		{
			cm_delete(windowToDestroy);
		}

		mScheduledForDestruction.clear();

		mMainWindow->update();

		for(auto& window : mEditorWindows)
		{
			window->update();
		}
	}
}