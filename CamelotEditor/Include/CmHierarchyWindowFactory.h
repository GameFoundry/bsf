#pragma once

#include "CmEditorPrerequisites.h"
#include <QtCore/QString>

namespace CamelotEditor
{
	class HierarchyWindowFactory
	{
	public:
		QtEditorWindow* create(QWidget* parent) const;

		const QString& getWindowName() const;
		const QString& getMenuCategory() const;
		const QString& getMenuItemName() const;
	};
}