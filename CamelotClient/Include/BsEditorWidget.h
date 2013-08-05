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
		void _changeParent(BS::GUIWidget& widget);

		void _disable();
		void _enable();

		static void destroy(EditorWidget* widget);

		boost::signal<void(EditorWidget*)> onClosed;
	protected:
		EditorWidget(const CM::WString& name);

		CM::WString mName;
		BS::GUIWidget* mParentWidget;
		BS::GUIArea* mContent;
	};
}