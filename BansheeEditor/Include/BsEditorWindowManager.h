#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT EditorWindowManager : public Module<EditorWindowManager>
	{
	public:
		EditorWindowManager();
		~EditorWindowManager();

		MainEditorWindow* createMain(const RenderWindowPtr& parentRenderWindow);
		EditorWindow* create();
		void registerWindow(EditorWindowBase* window);
		void destroy(EditorWindowBase* window);

		MainEditorWindow* getMainWindow() const { return mMainWindow; }

		void update();
	protected:
		MainEditorWindow* mMainWindow;

		Vector<EditorWindowBase*> mEditorWindows;
		Vector<EditorWindowBase*> mScheduledForDestruction;

		Vector<EditorWindowBase*> mEditorWindowsSnapshot;
	};
}