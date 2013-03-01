#pragma once

#include "CmEditorPrerequisites.h"
#include "CmEditorWindowFactory.h"
#include <QtCore/QString>

namespace CamelotEditor
{
	class HierarchyWindowFactory : public EditorWindowFactory
	{
	public:
		QtEditorWindow* create(QWidget* parent);

		const QString& getWindowName() const;
		const QString& getMenuCategory() const;
		const QString& getMenuItemName() const;
	};
}