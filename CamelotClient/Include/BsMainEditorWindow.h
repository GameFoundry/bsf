#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWindowBase.h"

namespace BansheeEditor
{
	class MainEditorWindow : public EditorWindowBase
	{
	public:
		~MainEditorWindow();

		void update();

		static MainEditorWindow* create(const CM::RenderWindowPtr& renderWindow);
	protected:
		friend class EditorWindowManager;
		MainEditorWindow(const CM::RenderWindowPtr& renderWindow);

	protected:
		GUIMenuBar* mMenuBar;
		DockManager* mDockManager;

		void itemDropped(CM::OSDropTarget& dropTarget, CM::INT32 x, CM::INT32 y);
		virtual void resized();

		void updateAreas();
	};
}