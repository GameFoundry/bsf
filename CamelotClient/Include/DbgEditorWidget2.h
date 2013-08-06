#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWidget.h"

namespace BansheeEditor
{
	class DbgEditorWidget2 : public EditorWidget
	{
	public:
		virtual ~DbgEditorWidget2();

		static DbgEditorWidget2* instance();
		static DbgEditorWidget2* open();
		static void close();

	protected:
		DbgEditorWidget2();

		void initialize();
	private:
		static DbgEditorWidget2* Instance;
	};
}