#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWindowBase.h"

namespace BansheeEditor
{
	class EditorWindow : public EditorWindowBase
	{
	public:
		EditorWindow();
		virtual ~EditorWindow();

		EditorWidgetContainer& getWidgets() const { return *mWidgets; }
	private:
		EditorWidgetContainer* mWidgets;
	};
}