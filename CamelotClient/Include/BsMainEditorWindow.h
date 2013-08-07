#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWindowBase.h"

namespace BansheeEditor
{
	class MainEditorWindow : public EditorWindowBase
	{
	public:
		MainEditorWindow(CM::RenderWindowPtr renderWindow);
		~MainEditorWindow();

	protected:
		DockManager* mDockManager;

		virtual void movedOrResized();
	};
}