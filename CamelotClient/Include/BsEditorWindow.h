#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWindowBase.h"

namespace BansheeEditor
{
	class EditorWindow : public EditorWindowBase
	{
	public:
		virtual ~EditorWindow();

		EditorWidgetContainer& getWidgets() const { return *mWidgets; }

		static EditorWindow& create();

	protected:
		EditorWindow();

		virtual void movedOrResized();
	private:
		EditorWidgetContainer* mWidgets;
	};
}