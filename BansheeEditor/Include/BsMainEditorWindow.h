#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWindowBase.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT MainEditorWindow : public EditorWindowBase
	{
	public:
		~MainEditorWindow();

		void update();

		/**
		 * @copydoc	EditorWindowBase::isMain
		 */
		virtual bool isMain() const { return true; }

		DockManager& getDockManager() const { return *mDockManager; }

		static MainEditorWindow* create(const RenderWindowPtr& renderWindow);
	protected:
		friend class EditorWindowManager;
		MainEditorWindow(const RenderWindowPtr& renderWindow);

	protected:
		GUIMenuBar* mMenuBar;
		DockManager* mDockManager;
		HProfilerOverlay mProfilerOverlay;

		virtual void resized();

		void updateAreas();
	};
}