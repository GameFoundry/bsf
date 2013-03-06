#include "CmHierarchyWidgetFactory.h"
#include "CmQtHierarchyWidget.h"

namespace CamelotEditor
{
	QtEditorWidget* HierarchyWidgetFactory::create(QWidget* parent)
	{
		return new QtHierarchyWidget(parent);
	}

	const QString& HierarchyWidgetFactory::getWindowName() const
	{
		static QString name = "Hierarchy";
		return name;
	}

	const QString& HierarchyWidgetFactory::getMenuCategory() const
	{
		static QString name = "Windows";
		return name;
	}

	const QString& HierarchyWidgetFactory::getMenuItemName() const
	{
		static QString name = "Hierarchy";
		return name;
	}
}