#include "CmQtSceneWidget.h"
#include "QtWidgets/QFrame"

namespace CamelotEditor
{
	QtSceneWidget::QtSceneWidget(QWidget* parent)
		:QtEditorWidget(parent, "Scene", tr("Scene"))
	{

	}

	QtSceneWidget::~QtSceneWidget() { }
}