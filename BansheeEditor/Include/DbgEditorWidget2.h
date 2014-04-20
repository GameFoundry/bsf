#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWidget.h"

namespace BansheeEngine
{
	class DbgEditorWidget2 : public EditorWidget<DbgEditorWidget2>
	{
	public:
		DbgEditorWidget2(const ConstructPrivately& dummy, EditorWidgetContainer& parentContainer);
		virtual ~DbgEditorWidget2();

		static DbgEditorWidget2* instance();
		static DbgEditorWidget2* open();
		static void close();
		static const String& getTypeName();

	private:
		static DbgEditorWidget2* Instance;
	};
}