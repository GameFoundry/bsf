#pragma once

#include "CmEditorPrerequisites.h"
#include "CmEditorWindowFactory.h"
#include <QtCore/QString>

namespace CamelotEditor
{
	class SceneWindowFactory : public EditorWindowFactory
	{
	public:
		QtEditorWindow* create(QWidget* parent);

		const QString& getWindowName() const;
		const QString& getMenuCategory() const;
		const QString& getMenuItemName() const;
	};
}