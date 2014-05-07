#pragma once

#include "BsEditorPrerequisites.h"
#include "CmModule.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	class EditorWindowManager : public Module<EditorWindowManager>
	{
	public:
		EditorWindowManager();
		~EditorWindowManager();

		MainEditorWindow* createMain(const RenderWindowPtr& parentRenderWindow);
		EditorWindow* create();
		void destroy(EditorWindowBase* window);

		MainEditorWindow* getMainWindow() const { return mMainWindow; }

		void update();
	protected:
		MainEditorWindow* mMainWindow;

		Vector<EditorWindowBase*> mEditorWindows;
		Vector<EditorWindowBase*> mScheduledForDestruction;
	};
}