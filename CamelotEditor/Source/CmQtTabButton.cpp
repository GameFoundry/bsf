#include "CmQtTabButton.h"
#include <QtGui/QMouseEvent>

namespace CamelotEditor
{
	QtTabButton::QtTabButton(QWidget* parent)
		:QPushButton(parent), mMouseDragging(false)
	{
		setupUi();
	}

	QtTabButton::~QtTabButton()
	{

	}

	void QtTabButton::setupUi()
	{
		connect(this, &QPushButton::clicked, this, &QtTabButton::buttonClicked);
	}

	void QtTabButton::mousePressEvent(QMouseEvent* event)
	{
		mMouseDragging = true;
		mMouseMoveStartPos = event->pos();
	}

	void QtTabButton::mouseReleaseEvent(QMouseEvent* event)
	{
		mMouseDragging = false;
	}

	void QtTabButton::mouseMoveEvent(QMouseEvent* event)
	{
		if(mMouseDragging)
		{
			QPoint diff = mMouseMoveStartPos - event->pos();

			// We currently handle undocking differently so this is not used
			//if(diff.manhattanLength() > 4)
			//	onDragged();
		}
	}	

	void QtTabButton::buttonClicked()
	{
		onSelected();
	}
}