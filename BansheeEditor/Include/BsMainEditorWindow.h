#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWindowBase.h"

namespace BansheeEngine
{
	class MainEditorWindow : public EditorWindowBase
	{
	public:
		~MainEditorWindow();

		void update();

		DockManager& getDockManager() const { return *mDockManager; }

		static MainEditorWindow* create(const RenderWindowPtr& renderWindow);
	protected:
		friend class EditorWindowManager;
		MainEditorWindow(const RenderWindowPtr& renderWindow);

	protected:
		GUIMenuBar* mMenuBar;
		DockManager* mDockManager;

		virtual void resized();

		void updateAreas();
	};
}