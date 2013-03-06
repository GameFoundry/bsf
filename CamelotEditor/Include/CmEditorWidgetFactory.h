#pragma once

#include "CmEditorPrerequisites.h"
#include <QtCore/QString>

namespace CamelotEditor
{
	class EditorWidgetFactory
	{
	public:
		virtual QtEditorWidget* create(QWidget* parent) = 0;

		virtual const QString& getWindowName() const = 0;
		virtual const QString& getMenuCategory() const = 0;
		virtual const QString& getMenuItemName() const = 0;
	};
}