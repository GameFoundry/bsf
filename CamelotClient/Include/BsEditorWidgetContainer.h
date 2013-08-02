#pragma once

#include "BsEditorPrerequisites.h"

namespace BansheeEditor
{
	class EditorWidgetContainer
	{
	public:
		EditorWidgetContainer();
		virtual ~EditorWidgetContainer();

		void add(EditorWidget& widget);
		void remove(EditorWidget& widget);
		void move(EditorWidget& widget, CM::UINT32 newPosition);
	};
}