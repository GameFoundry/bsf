#include "CmQtSceneWidget.h"
#include "QtWidgets/QFrame"

namespace CamelotEditor
{
	QtSceneWidget::QtSceneWidget(QWidget* parent)
		:QtEditorWidget(parent, "Scene", tr("Scene"), QSize(200, 300))
	{
		resize(getDefaultSize());
	}

	QtSceneWidget::~QtSceneWidget() { }
}