#pragma once

#include "CmEditorPrerequisites.h"
#include <QtWidgets/QPushButton>
#include <boost/signal.hpp>

namespace CamelotEditor
{
	class QtTabButton : public QPushButton
	{
		Q_OBJECT

	public:
		QtTabButton(QWidget* parent);
		virtual ~QtTabButton();

		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);

		boost::signal<void()> onDragged;
		boost::signal<void()> onSelected;
	private:
		QPoint mMouseMoveStartPos;
		bool mMouseDragging;

		void setupUi();

		void buttonClicked();
	};
}