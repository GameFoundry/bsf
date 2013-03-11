#include "CmQtEditorWidget.h"

namespace CamelotEditor
{
	QtEditorWidget::QtEditorWidget(QWidget* parent, const QString& name, const QString& title, const QSize& defaultSize)
		:QWidget(parent), mName(name), mTitle(title), mDefaultSize(defaultSize)
	{

	}

	void QtEditorWidget::closeWidget()
	{
		onClosed(this);
		destroy();
	}
}