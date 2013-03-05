#pragma once

#include "CmEditorPrerequisites.h"
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
		QtEditorWindow(QWidget* parent, const QString& name, const QString& title);
		virtual ~QtEditorWindow() { }

		const QString& getName() const { return mName; }

		void undock();
		void dock();

		bool isDocked() const { return mIsDocked; }
		WindowLayoutDesc getLayoutDesc() const;
		void restoreFromLayoutDesc(const WindowLayoutDesc& desc);

		boost::signal<void(QtEditorWindow*)> onClosed;
	protected:
		QWidget* mContentWidget;

		QSizePolicy	sizePolicy() const;

	private:
		ResizeMode mResizeMode;
		bool mMoveMode;
		QPoint mDragOffset;
		QWidget* mTitleBar;
		QLabel* mLblTitle;
		QPushButton* mBtnClose;
		QWidget* mCentralWidget;
		QTimer* mTimer;
		bool mIsDocked;
		QString mName;

		void setupUi(QString title);
		void setupSignals();
		void retranslateUi(QString title);
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
		void closeWindow();
	};
}