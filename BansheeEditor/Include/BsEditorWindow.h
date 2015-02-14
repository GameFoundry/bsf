#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWindowBase.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT EditorWindow : public EditorWindowBase
	{
	public:
		virtual ~EditorWindow();

		EditorWidgetContainer& widgets() const { return *mWidgets; }

		virtual void update();

		static EditorWindow* create();

	protected:
		friend class EditorWindowManager;
		EditorWindow();

		virtual void resized();
	private:
		EditorWidgetContainer* mWidgets;

		void updateSize();
		void widgetRemoved();
		void closeWindowDelayed();
	};
}