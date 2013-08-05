#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWidget.h"

namespace BansheeEditor
{
	class DbgEditorWidget1 : public EditorWidget
	{
	public:
		virtual ~DbgEditorWidget1();

		static DbgEditorWidget1* instance();
		static DbgEditorWidget1* open();
		static void close();

	protected:
		DbgEditorWidget1();

		void initialize();
	private:
		static DbgEditorWidget1* Instance;
	};
}