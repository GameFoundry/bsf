#pragma once

#include "BsEditorPrerequisites.h"
#include "CmModule.h"
#include <boost/signals.hpp>

namespace BansheeEditor
{
	class EditorWindowManager : public CM::Module<EditorWindowManager>
	{
	public:
		EditorWindowManager();
		~EditorWindowManager();

		MainEditorWindow* createMain(const CM::RenderWindowPtr& parentRenderWindow);
		EditorWindow* create();
		void destroy(EditorWindowBase* window);

		MainEditorWindow* getMainWindow() const { return mMainWindow; }

		void update();
	protected:
		MainEditorWindow* mMainWindow;

		CM::Vector<EditorWindowBase*>::type mEditorWindows;
		CM::Vector<EditorWindowBase*>::type mScheduledForDestruction;
	};
}