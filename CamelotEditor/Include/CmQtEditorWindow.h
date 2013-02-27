#pragma once

#include "CmPrerequisites.h"
#include <QtWidgets/QWidget>
#include <QtCore/QPoint>

namespace CamelotEditor
{
	// TODO - Low priority. When resizing from left/top, the window resize noticeably lags behind
	// TODO - Low priority. When resizing from left/top, if a minimum size is reached, the window will just move as if it's being dragged
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
		QtEditorWindow(QWidget* parent, QWidget* content, const QString& title);
		virtual ~QtEditorWindow() { }

		void undock();
		void dock();

		bool isDocked() { return mIsDocked; }

	protected:
		QWidget* mContent;

		QSizePolicy	sizePolicy() const;

	private:
		ResizeMode mResizeMode;
		bool mMoveMode;
		QPoint mDragOffset;
		QWidget* mTitleBar;
		QWidget* mCentralWidget;
		bool mIsDocked;

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
		void closeWindow();
	};
}