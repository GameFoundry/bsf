#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWidget.h"

namespace BansheeEngine
{
	class DbgEditorWidget1 : public EditorWidget<DbgEditorWidget1>
	{
	public:
		DbgEditorWidget1(const ConstructPrivately& dummy, EditorWidgetContainer& parentContainer);
		virtual ~DbgEditorWidget1();

		virtual void _update();

		static DbgEditorWidget1* instance();
		static DbgEditorWidget1* open();
		static void close();
		static const String& getTypeName();

	private:
		static DbgEditorWidget1* Instance;
		GUISceneTreeView* mSceneTreeView = nullptr;
		GUIResourceTreeView* mResourceTreeView = nullptr;
	};
}