#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWidget.h"

namespace BansheeEditor
{
	class DbgEditorWidget1 : public EditorWidget<DbgEditorWidget1>
	{
	public:
		DbgEditorWidget1(const ConstructPrivately& dummy);
		virtual ~DbgEditorWidget1();

		static DbgEditorWidget1* instance();
		static DbgEditorWidget1* open();
		static void close();
		static const CM::String& getTypeName();

	protected:
		void initialize();

	private:
		static DbgEditorWidget1* Instance;
	};
}