#include "CmHierarchyWindowFactory.h"
#include "CmQtHierarchyWindow.h"

namespace CamelotEditor
{
	QtEditorWindow* HierarchyWindowFactory::create(QWidget* parent)
	{
		return new QtHierarchyWindow(parent);
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