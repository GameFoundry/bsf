#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWidget.h"

namespace BansheeEditor
{
	class DbgEditorWidget2 : public EditorWidget<DbgEditorWidget2>
	{
	public:
		DbgEditorWidget2(const ConstructPrivately& dummy);
		virtual ~DbgEditorWidget2();

		static DbgEditorWidget2* instance();
		static DbgEditorWidget2* open();
		static void close();
		static const CM::String& getTypeName();

	protected:
		void initialize();

	private:
		static DbgEditorWidget2* Instance;
	};
}