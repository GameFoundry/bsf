#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWindowBase.h"

namespace BansheeEditor
{
	class EditorWindow : public EditorWindowBase
	{
	public:
		virtual ~EditorWindow();

		EditorWidgetContainer& widgets() const { return *mWidgets; }

		static EditorWindow* create();

	protected:
		friend class EditorWindowManager;
		EditorWindow();

		virtual void movedOrResized();
	private:
		EditorWidgetContainer* mWidgets;

		void widgetRemoved();
	};
}