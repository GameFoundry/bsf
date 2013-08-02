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

		virtual void _initialize(BS::GUIWidget& widget, CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height);
	private:
		static std::shared_ptr<DbgEditorWidget1> Instance;
	};
}