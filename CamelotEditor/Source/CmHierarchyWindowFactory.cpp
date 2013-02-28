#include "CmHierarchyWindowFactory.h"
#include "CmQtSceneWindow.h"

namespace CamelotEditor
{
	QtEditorWindow* HierarchyWindowFactory::create(QWidget* parent) const
	{
		return new QtSceneWindow(parent);
	}

	const QString& HierarchyWindowFactory::getWindowName() const
	{
		static QString name = "Hierarchy";
		return name;
	}

	const QString& HierarchyWindowFactory::getMenuCategory() const
	{
		static QString name = "Windows";
		return name;
	}

	const QString& HierarchyWindowFactory::getMenuItemName() const
	{
		static QString name = "Hierarchy";
		return name;
	}
}