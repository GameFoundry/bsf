#pragma once

#include "CmEditorPrerequisites.h"
#include "CmEditorWidgetFactory.h"
#include <QtCore/QString>

namespace CamelotEditor
{
	class SceneWidgetFactory : public EditorWidgetFactory
	{
	public:
		QtEditorWidget* create(QWidget* parent);

		const QString& getWindowName() const;
		const QString& getMenuCategory() const;
		const QString& getMenuItemName() const;
	};
}