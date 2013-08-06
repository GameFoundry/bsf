#pragma once

#include "BsEditorPrerequisites.h"
#include "boost/signal.hpp"

namespace BansheeEditor
{
	class EditorWidget
	{
	public:
		virtual ~EditorWidget();

		const CM::WString& getName() const { return mName; }

		void _setSize(CM::UINT32 width, CM::UINT32 height);
		void _setPosition(CM::INT32 x, CM::INT32 y);
		void _changeParent(EditorWidgetContainer* parent);

		void _disable();
		void _enable();

		static void destroy(EditorWidget* widget);
	protected:
		EditorWidget(const CM::WString& name);

		CM::WString mName;
		EditorWidgetContainer* mParent;
		BS::GUIArea* mContent;

		BS::GUIWidget& getParentWidget() const;
	};
}