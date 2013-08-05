#include "BsEditorWindowManager.h"
#include "BsEditorWindow.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	EditorWindowManager::~EditorWindowManager()
	{
		while(mEditorWindows.size() > 0)
			destroy(mEditorWindows[0]);
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

		mEditorWindows.erase(iterFind);
		cm_delete(window);
	}
}