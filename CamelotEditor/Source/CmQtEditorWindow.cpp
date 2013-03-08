#include "CmQtEditorWindow.h"

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStackedWidget>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtCore/QTimer>

#include "CmDebug.h"
#include "CmQtEditorWidget.h"
#include "CmQtDynamicTabBar.h"
#include "CmWindowDockManager.h"
#include "CmEditorWindowManager.h"

namespace CamelotEditor
{
	QtEditorWindow::QtEditorWindow(QWidget* parent, INT32 id)
		:QWidget(parent), mResizeMode(RM_NONE), mMoveMode(false), mIsDocked(false), mId(id), mActiveWidgetIdx(0)
	{
		setupUi();
	}

	void QtEditorWindow::setupUi()
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

		mTabBar = new QtDynamicTabBar(this);
		
		mBtnClose = new QPushButton(this);
		
		QHBoxLayout* titleLayout = new QHBoxLayout(this);
		titleLayout->setMargin(0);
		titleLayout->addWidget(mTabBar, 1);
		titleLayout->addWidget(mBtnClose);
		mTitleBar->setLayout(titleLayout);

		/************************************************************************/
		/* 							CENTRAL LAYOUT                      		*/
		/************************************************************************/

		mCentralWidget = new QWidget(this);
		mStackedWidget = new QStackedWidget(this);

		QVBoxLayout* centralLayout = new QVBoxLayout(this);
		centralLayout->setMargin(0);
		centralLayout->addWidget(mCentralWidget);
		setLayout(centralLayout);

		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->setMargin(0);
		mainLayout->setContentsMargins(2, 0, 2, 2);
		mainLayout->setSpacing(0);
		mainLayout->addWidget(mTitleBar);
		mainLayout->addWidget(mStackedWidget, 1);
		mCentralWidget->setLayout(mainLayout);
		
		setObjectNames();
		retranslateUi();
		setupSignals();
	}

	void QtEditorWindow::setupSignals()
	{
		connect(mBtnClose, SIGNAL(clicked()), this, SLOT(closeWidget()));
		connect(mTimer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
		
		mTabBar->onTabSelected.connect(boost::bind(&QtEditorWindow::setActiveWidget, this, _1));
	}

	void QtEditorWindow::retranslateUi()
	{
		mBtnClose->setText(tr("Close"));
	}

	void QtEditorWindow::setObjectNames()
	{
		mTitleBar->setObjectName(QStringLiteral("TitleBar"));
		mTabBar->setObjectName(QStringLiteral("TabBar"));
		mBtnClose->setObjectName(QStringLiteral("BtnClose"));

		mCentralWidget->setObjectName(QStringLiteral("CentralWidget"));
		mStackedWidget->setObjectName(QStringLiteral("StackedWidget"));
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

	WindowLayoutDesc QtEditorWindow::getLayoutDesc() const
	{
		WindowLayoutDesc desc;
		desc.width = geometry().width();
		desc.height = geometry().height();
		desc.left = geometry().x();
		desc.top = geometry().y();
		desc.screenIdx = -1;
		desc.id = getId();
		desc.maximized = false;
		desc.dockState = WDS_FLOATING;

		if(isDocked())
		{
			WindowDragDropLocation dockLocation = gWindowDockManager().getDockLocation(this);

			switch(dockLocation)
			{
			case CM_WINDROP_LEFT:
				desc.dockState = WDS_LEFT;
				break;
			case CM_WINDROP_RIGHT:
				desc.dockState = WDS_RIGHT;
				break;
			case CM_WINDROP_BOTTOM:
				desc.dockState = WDS_BOTTOM;
				break;
			case CM_WINDROP_TOP:
				desc.dockState = WDS_TOP;
				break;
			case CM_WINDROP_CENTER:
				desc.dockState = WDS_CENTER;
				break;
			default:
				assert(false);
			}

			desc.dockParentId = gWindowDockManager().getDockParentId(this);
		}
		else 
		{
			desc.dockState = WDS_FLOATING;
			desc.dockParentId = -1;
		}

		desc.activeWidget = mActiveWidgetIdx;

		for(auto iter = mEditorWidgets.begin(); iter != mEditorWidgets.end(); ++iter)
			desc.childWidgetNames.push_back((*iter)->getName());

		return desc;
	}

	void QtEditorWindow::restoreFromLayoutDesc(const WindowLayoutDesc& desc)
	{
		setGeometry(desc.left, desc.top, desc.width, desc.height);
		mId = desc.id;

		for(auto iter2 = desc.childWidgetNames.begin(); iter2 != desc.childWidgetNames.end(); ++iter2)
			gEditorWindowManager().openWidget(*iter2, this);

		if(desc.dockState != WDS_FLOATING)
		{
			WindowDragDropLocation dockLocation = CM_WINDROP_NONE;
			switch(desc.dockState)
			{
			case WDS_LEFT:
				dockLocation = CM_WINDROP_LEFT;
				break;
			case WDS_RIGHT:
				dockLocation = CM_WINDROP_RIGHT;
				break;
			case WDS_BOTTOM:
				dockLocation = CM_WINDROP_BOTTOM;
				break;
			case WDS_TOP:
				dockLocation = CM_WINDROP_TOP;
				break;
			case WDS_CENTER:
				dockLocation = CM_WINDROP_CENTER;
				break;
			}

			QtEditorWindow* dockParent = nullptr;

			if(desc.dockParentId != -1)
				dockParent = gEditorWindowManager().getOpenWindow(desc.dockParentId);
				
			gWindowDockManager().dockWindow(this, dockParent, dockLocation);
		}

		setActiveWidget(desc.activeWidget);
	}

	void QtEditorWindow::addWidget(QtEditorWidget* widget)
	{
		assert(widget != nullptr);

		auto iterFind = std::find(mEditorWidgets.begin(), mEditorWidgets.end(), widget);
		if(iterFind != mEditorWidgets.end())
			CM_EXCEPT(InvalidParametersException, "Specified widget already exists in this window.");

		widget->setParentWindow(this);
		mEditorWidgets.push_back(widget);
		mTabBar->addTab(widget->getName());
	}

	void QtEditorWindow::insertWidget(UINT32 idx, QtEditorWidget* widget)
	{
		assert(widget != nullptr);

		auto iterFind = std::find(mEditorWidgets.begin(), mEditorWidgets.end(), widget);
		if(iterFind != mEditorWidgets.end())
			CM_EXCEPT(InvalidParametersException, "Specified widget already exists in this window.");

		if(idx > (UINT32)mEditorWidgets.size())
			idx = (UINT32)mEditorWidgets.size();

		widget->setParentWindow(this);
		mEditorWidgets.insert(mEditorWidgets.begin() + idx, widget);
		mTabBar->insertTab(idx, widget->getName());
	}

	void QtEditorWindow::removeWidget(UINT32 idx)
	{
		if(idx >= (UINT32)mEditorWidgets.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) +". Valid range: 0 .. " + toString((UINT32)mEditorWidgets.size()));

		mEditorWidgets.erase(mEditorWidgets.begin() + idx);
		mTabBar->removeTab(idx);
	}

	QtEditorWidget* QtEditorWindow::getWidget(UINT32 idx) const
	{
		return mEditorWidgets.at(idx);
	}

	void QtEditorWindow::setActiveWidget(UINT32 idx)
	{
		if(idx >= (UINT32)mEditorWidgets.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) +". Valid range: 0 .. " + toString((UINT32)mEditorWidgets.size()));

		mActiveWidgetIdx = idx;

		// TODO
	}

	QWidget* QtEditorWindow::getContentWidget() const
	{ 
		return mStackedWidget; 
	}

	QWidget* QtEditorWindow::getTabWidget() const
	{ 
		return mTabBar; 
	}

	std::vector<QPolygon> QtEditorWindow::getTabBarDropLocations() const
	{
		return mTabBar->getDropLocations();
	}

	INT32 QtEditorWindow::getActiveTabBarDropLocation(const QPoint& mousePos) const
	{
		std::vector<QPolygon> dropLocations = getTabBarDropLocations();

		INT32 idx = 0;
		for(auto iter = dropLocations.begin(); iter != dropLocations.end(); ++iter)
		{
			if(iter->containsPoint(mousePos, Qt::OddEvenFill))
				return idx;

			idx++;
		}

		return -1;
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
		gWindowDockManager().windowClosed(this); // TODO - Hook this up to use onClosed signal as well
		onClosed(this);
		close();
	}

	void QtEditorWindow::closeWidget()
	{
		QtEditorWidget* activeWidget = getWidget(mActiveWidgetIdx);
		removeWidget(mActiveWidgetIdx);

		activeWidget->closeWidget();

		if(getNumWidgets() == 0)
			closeWindow();		
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