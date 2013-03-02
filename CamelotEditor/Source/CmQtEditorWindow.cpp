#include "CmQtEditorWindow.h"

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtCore/QTimer>

#include "CmDebug.h"
#include "CmWindowDockManager.h"

namespace CamelotEditor
{
	QtEditorWindow::QtEditorWindow(QWidget* parent, const QString& title)
		:QWidget(parent), mResizeMode(RM_NONE), mMoveMode(false), mIsDocked(false)
	{
		setupUi(title);
	}

	void QtEditorWindow::setupUi(QString title)
	{
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
		
		//setAttribute(Qt::WA_NoBackground, true);
		//setAutoFillBackground(false);
		// 
		mTimer = new QTimer(this);

		/************************************************************************/
		/* 								TITLE BAR	                     		*/
		/************************************************************************/
		mTitleBar = new QWidget(this);
		mTitleBar->setMouseTracking(true);

		mLblTitle = new QLabel(this);
		mLblTitle->setAttribute(Qt::WA_TransparentForMouseEvents);
		
		mBtnClose = new QPushButton(this);
		
		QHBoxLayout* titleLayout = new QHBoxLayout(this);
		titleLayout->setMargin(0);
		titleLayout->addWidget(mLblTitle, 1);
		titleLayout->addWidget(mBtnClose);
		mTitleBar->setLayout(titleLayout);

		/************************************************************************/
		/* 							CENTRAL LAYOUT                      		*/
		/************************************************************************/

		mCentralWidget = new QWidget(this);

		mContentWidget = new QWidget(this);

		QVBoxLayout* centralLayout = new QVBoxLayout(this);
		centralLayout->setMargin(0);
		centralLayout->addWidget(mCentralWidget);
		setLayout(centralLayout);

		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->setMargin(0);
		mainLayout->setContentsMargins(2, 0, 2, 2);
		mainLayout->setSpacing(0);
		mainLayout->addWidget(mTitleBar);
		mainLayout->addWidget(mContentWidget, 1);
		mCentralWidget->setLayout(mainLayout);
		
		setObjectNames();
		retranslateUi(title);
		setupSignals();
	}

	void QtEditorWindow::setupSignals()
	{
		connect(mBtnClose, SIGNAL(clicked()), this, SLOT(closeWindow()));
		connect(mTimer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
	}

	void QtEditorWindow::retranslateUi(QString title)
	{
		mLblTitle->setText(title);
		mBtnClose->setText(tr("Close"));
	}

	void QtEditorWindow::setObjectNames()
	{
		mTitleBar->setObjectName(QStringLiteral("TitleBar"));
		mLblTitle->setObjectName(QStringLiteral("LblTitle"));
		mBtnClose->setObjectName(QStringLiteral("BtnClose"));

		mCentralWidget->setObjectName(QStringLiteral("CentralWidget"));
		mContentWidget->setObjectName(QStringLiteral("ContentWidget"));
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

	void QtEditorWindow::enterEvent(QEvent *e)
	{
		mTimer->start(100);
	}

	void QtEditorWindow::leaveEvent(QEvent *e)
	{
		mTimer->stop();
	}

	void QtEditorWindow::mousePressEvent(QMouseEvent* event)
	{
		if(event->buttons() & Qt::LeftButton)
		{
			if(isOverResizeArea(event->globalPos()))
			{
				//setUpdatesEnabled(false);
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
		//setUpdatesEnabled(true);

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

				int leftOld = left;
				int topOld = top;

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

				// Check if we have reached width or height limit. We need to do some special
				// steps otherwise the window will move (instead of resize) after the limit is reached
				if(width < minimumWidth())
				{
					left = leftOld;
					width = minimumWidth();
				}

				if(height < minimumHeight())
				{
					top = topOld;
					height = minimumHeight();
				}

				setGeometry(left, top, width, height);

				update();
			}
		}

		QWidget::mouseMoveEvent(event);
	}

	void QtEditorWindow::timerUpdate()
	{
		if(!isDocked())
		{
			if(underMouse())
			{
				ResizeMode resizeMode = getResizeMode(QCursor::pos());

				switch(resizeMode)
				{
				case RM_LEFT:
					setCursor(Qt::SizeHorCursor);
					break;
				case RM_RIGHT:
					setCursor(Qt::SizeHorCursor);
					break;
				case RM_TOP:
					setCursor(Qt::SizeVerCursor);
					break;
				case RM_BOTTOM:
					setCursor(Qt::SizeVerCursor);
					break;
				case (ResizeMode)(RM_BOTTOM | RM_RIGHT):
					setCursor(Qt::SizeFDiagCursor);
					break;
				case (ResizeMode)(RM_TOP | RM_LEFT):
					setCursor(Qt::SizeFDiagCursor);
					break;
				case (ResizeMode)(RM_BOTTOM | RM_LEFT):
					setCursor(Qt::SizeBDiagCursor);
					break;
				case (ResizeMode)(RM_TOP | RM_RIGHT):
					setCursor(Qt::SizeBDiagCursor);
					break;
				case RM_NONE:
					setCursor(Qt::ArrowCursor);
					break;
				}
			}
			else
			{
				if(cursor().shape() != Qt::ArrowCursor)
					unsetCursor();
			}
		}
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