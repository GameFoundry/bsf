#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWidget.h"

namespace BansheeEditor
{
	class DbgEditorWidget1 : public EditorWidget
	{
	public:
		virtual ~DbgEditorWidget1();

		static std::shared_ptr<DbgEditorWidget1> instance();
		static std::shared_ptr<DbgEditorWidget1> open();
		static void close();

	protected:
		DbgEditorWidget1();

		void initialize();
	private:
		static std::shared_ptr<DbgEditorWidget1> Instance;
	};
}