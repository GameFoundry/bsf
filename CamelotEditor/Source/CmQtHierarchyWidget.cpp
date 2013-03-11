#include "CmQtHierarchyWidget.h"

namespace CamelotEditor
{
	QtHierarchyWidget::QtHierarchyWidget(QWidget* parent)
		:QtEditorWidget(parent, "Hierarchy", tr("Hierarchy"), QSize(200, 300))
	{
		resize(getDefaultSize());
	}

	QtHierarchyWidget::~QtHierarchyWidget() { }
}