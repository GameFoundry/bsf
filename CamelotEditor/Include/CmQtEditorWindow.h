#pragma once

#include "CmEditorPrerequisites.h"
#include "CmQtEditorWidget.h"
#include "CmEditorPrefs.h"
#include <QtWidgets/QWidget>
#include <QtCore/QPoint>
#include <boost/signal.hpp>

namespace CamelotEditor
{
	// TODO - Low priority. When resizing from left/top, the window resize noticeably lags behind
	// TODO - Low priority. When resizing or moving, the top/right sides seem to redraw a frame too late and borders appear invisible for a split second
	class QtEditorWindow : public QWidget
	{
		Q_OBJECT

		enum ResizeMode
		{
			RM_NONE = 0x00,
			RM_LEFT = 0x01,
			RM_RIGHT = 0x02,
			RM_TOP = 0x04,
			RM_BOTTOM = 0x08
		};

	public:
		QtEditorWindow(QWidget* parent, INT32 id);
		virtual ~QtEditorWindow() { }

		void undock();
		void dock();

		bool isDocked() const { return mIsDocked; }
		UINT32 getId() const { return mId; }

		WindowLayoutDesc getLayoutDesc() const;
		void restoreFromLayoutDesc(const WindowLayoutDesc& desc);

		void addWidget(QtEditorWidget* widget);
		void insertWidget(UINT32 idx, QtEditorWidget* widget);
		void removeWidget(UINT32 idx);
		UINT32 getNumWidgets() const { return (UINT32)mEditorWidgets.size(); }
		QtEditorWidget* getWidget(UINT32 idx) const;

		void setActiveWidget(UINT32 idx);
		QWidget* getContentWidget() const;
		QWidget* getTabWidget() const;

		std::vector<QPolygon> getTabBarDropLocations() const;
		INT32 getActiveTabBarDropLocation(const QPoint& mousePos) const;

		void closeWindow();

		boost::signal<void(QtEditorWindow*)> onClosed;
	protected:
		QSizePolicy	sizePolicy() const;

	private:
		INT32 mId;
		ResizeMode mResizeMode;
		bool mMoveMode;
		QPoint mDragOffset;
		QWidget* mTitleBar;
		QtDynamicTabBar* mTabBar;
		QPushButton* mBtnClose;
		QWidget* mCentralWidget;
		QStackedWidget* mStackedWidget;
		QTimer* mTimer;
		bool mIsDocked;

		vector<QtEditorWidget*>::type mEditorWidgets;
		UINT32 mActiveWidgetIdx;

		void setupUi();
		void setupSignals();
		void retranslateUi();
		void setObjectNames();

		void enterEvent(QEvent *e);
		void leaveEvent(QEvent *e);

		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);

		ResizeMode getResizeMode(QPoint mousePos);
		bool isOverResizeArea(QPoint mousePos);

		/**
		 * @brief	Query if 'mousePos' is over the area that can be used for dragging the window around.
		 *
		 * @param	mousePos	The mouse position, in global coordinates.
		 */
		bool isOverDragArea(QPoint mousePos);

		static const int RESIZE_BORDER_SIZE = 2;
		static const int TITLE_BAR_HEIGHT = 20;

		/************************************************************************/
		/* 								SLOTS		                     		*/
		/************************************************************************/
	private Q_SLOTS:
		void timerUpdate();
		void closeWidget();
	};
}