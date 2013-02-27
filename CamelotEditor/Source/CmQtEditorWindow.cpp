#include "CmQtEditorWindow.h"

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>

#include "CmDebug.h"
#include "CmWindowDockManager.h"

using namespace CamelotEngine;

namespace CamelotEditor
{
	QtEditorWindow::QtEditorWindow(QWidget* parent, QWidget* content, const QString& title)
		:QWidget(parent), mContent(content), mResizeMode(RM_NONE), mMoveMode(false), mIsDocked(false)
	{
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

		/************************************************************************/
		/* 								TITLE BAR	                     		*/
		/************************************************************************/
		mTitleBar = new QWidget();
		mTitleBar->setObjectName("titleBar");

		QLabel* titleLabel = new QLabel();
		titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
		titleLabel->setText(title);

		QPushButton* closeBtn = new QPushButton();
		closeBtn->setText("Close");

		QHBoxLayout* titleLayout = new QHBoxLayout();
		titleLayout->setMargin(0);
		titleLayout->addWidget(titleLabel, 1);
		titleLayout->addWidget(closeBtn);
		mTitleBar->setLayout(titleLayout);

		/************************************************************************/
		/* 							CENTRAL LAYOUT                      		*/
		/************************************************************************/

		mCentralWidget = new QWidget();
		mCentralWidget->setObjectName("window");

		QVBoxLayout* centralLayout = new QVBoxLayout();
		centralLayout->setMargin(0);
		centralLayout->addWidget(mCentralWidget);
		setLayout(centralLayout);

		QVBoxLayout* mainLayout = new QVBoxLayout();
		mainLayout->setMargin(0);
		mainLayout->setContentsMargins(2, 0, 2, 2);
		mainLayout->setSpacing(0);
		mainLayout->addWidget(mTitleBar);
		mainLayout->addWidget(mContent, 1);
		mCentralWidget->setLayout(mainLayout);

		/************************************************************************/
		/* 								OTHER		                      		*/
		/************************************************************************/

		// Style sheets // TODO - Load this from disk from unified storage
		mTitleBar->setStyleSheet("QWidget#titleBar { background-color : rgb(157, 208, 250) }");
		mCentralWidget->setStyleSheet("#window { border-style: solid; border-width: 2px; border-color: rgb(157, 208, 250); }");

		//setAttribute(Qt::WA_TransparentForMouseEvents);
		
		connect(closeBtn, SIGNAL(clicked()), this, SLOT(closeWindow()));
	}

	QSizePolicy	QtEditorWindow::sizePolicy() const
	{
		return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	}

	void QtEditorWindow::undock()
	{
		if(mIsDocked)
		{
			setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
			mIsDocked = false;
		}
	}

	void QtEditorWindow::dock()
	{
		if(!mIsDocked)
		{
			setWindowFlags(Qt::Widget);
			mIsDocked = true;
		}
	}

	void QtEditorWindow::mousePressEvent(QMouseEvent* event)
	{
		if(event->buttons() & Qt::LeftButton)
		{
			if(isOverResizeArea(event->globalPos()))
			{
				mResizeMode = getResizeMode(event->globalPos());
			}
			else if(isOverDragArea(event->globalPos()))
			{
				mMoveMode = true;

				QPoint globalTopLeft = mapToGlobal(QPoint(0, 0));
				mDragOffset = QPoint(globalTopLeft.x() - event->globalPos().x(), globalTopLeft.y() - event->globalPos().y());
			}

			event->accept();
		}

		QWidget::mousePressEvent(event);
	}

	void QtEditorWindow::mouseReleaseEvent(QMouseEvent* event)
	{
		if(mMoveMode)
		{
			gWindowDockManager().windowReleased(this, event->globalPos());
		}

		mMoveMode = false;
		mResizeMode = RM_NONE;

		QWidget::mouseReleaseEvent(event);
	}

	void QtEditorWindow::mouseMoveEvent(QMouseEvent* event)
	{
		if(mMoveMode)
		{
			QPoint globalPos = mapToGlobal(event->pos());
			gWindowDockManager().windowDragged(this, globalPos);

			if(!mIsDocked)
				move(event->globalPos() + mDragOffset);

			event->accept();
		}
		else if(mResizeMode != RM_NONE)
		{
			if(!isDocked())
			{
				int left = geometry().left();
				int width = geometry().width();
				int top = geometry().top();
				int height = geometry().height();

				if (mResizeMode & RM_LEFT)
				{
					width = width + (left - event->globalPos().x());
					left = event->globalPos().x();
				}

				if(mResizeMode & RM_RIGHT)
				{
					width = event->globalPos().x() - left;
				}

				if(mResizeMode & RM_TOP)
				{
					height = height + (top - event->globalPos().y());
					top = event->globalPos().y();
				}

				if(mResizeMode & RM_BOTTOM)
				{
					height = event->globalPos().y() - top;
				}

				setGeometry(left, top, width, height);
				updateGeometry();
			}
		}

		QWidget::mouseMoveEvent(event);
	}

	void QtEditorWindow::closeWindow()
	{
		gWindowDockManager().windowClosed(this);
		close();
	}

	QtEditorWindow::ResizeMode QtEditorWindow::getResizeMode(QPoint mousePos)
	{
		QPoint localMousePos = mapFromGlobal(mousePos);

		QPoint topLeft = mapFromParent(geometry().topLeft());
		QPoint botRight = mapFromParent(geometry().bottomRight());

		int distFromRight = botRight.x() - localMousePos.x();
		int distFromLeft = localMousePos.x() - topLeft.x();
		int distFromBot = botRight.y() - localMousePos.y();
		int distFromTop = localMousePos.y() - topLeft.y();

		if(distFromRight <= RESIZE_BORDER_SIZE && distFromRight >= 0 && distFromBot <= RESIZE_BORDER_SIZE && distFromBot >= 0)
			return (ResizeMode)(RM_BOTTOM | RM_RIGHT);

		if(distFromLeft <= RESIZE_BORDER_SIZE && distFromLeft >= 0 && distFromTop <= RESIZE_BORDER_SIZE && distFromTop >= 0)
			return (ResizeMode)(RM_TOP | RM_LEFT);

		if(distFromLeft <= RESIZE_BORDER_SIZE && distFromLeft >= 0 && distFromBot <= RESIZE_BORDER_SIZE && distFromBot >= 0)
			return (ResizeMode)(RM_BOTTOM | RM_LEFT);

		if(distFromRight <= RESIZE_BORDER_SIZE && distFromRight >= 0 && distFromTop <= RESIZE_BORDER_SIZE && distFromTop >= 0)
			return (ResizeMode)(RM_TOP | RM_RIGHT);

		if(distFromRight <= RESIZE_BORDER_SIZE && distFromRight >= 0)
			return RM_RIGHT;

		if(distFromLeft <= RESIZE_BORDER_SIZE && distFromLeft >= 0)
			return RM_LEFT;

		if(distFromBot <= RESIZE_BORDER_SIZE && distFromBot >= 0)
			return RM_BOTTOM;

		if(distFromTop <= RESIZE_BORDER_SIZE && distFromTop >= 0)
			return RM_TOP;

		return RM_NONE;
	}

	bool QtEditorWindow::isOverResizeArea(QPoint mousePos)
	{
		QPoint localMousePos = mapFromGlobal(mousePos);

		int distFromRight = geometry().width() - localMousePos.x();
		int distFromLeft = localMousePos.x();
		int distFromBot = geometry().height() - localMousePos.y();
		int distFromTop = localMousePos.y();

		if(distFromRight <= RESIZE_BORDER_SIZE && distFromRight >= 0)
			return true;

		if(distFromLeft <= RESIZE_BORDER_SIZE && distFromLeft >= 0)
			return true;

		if(distFromBot <= RESIZE_BORDER_SIZE && distFromBot >= 0)
			return true;

		if(distFromTop <= RESIZE_BORDER_SIZE && distFromTop >= 0)
			return true;

		return false;
	}

	bool QtEditorWindow::isOverDragArea(QPoint mousePos)
	{
		if(getResizeMode(mousePos) != RM_NONE)
			return false;

		QPoint localMousePos = mapFromGlobal(mousePos);
		int distFromTop = localMousePos.y();

		if(distFromTop <= TITLE_BAR_HEIGHT)
			return true;

		return false;
	}
}