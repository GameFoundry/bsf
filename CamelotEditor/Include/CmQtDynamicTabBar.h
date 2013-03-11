#pragma once

#include "CmEditorPrerequisites.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <boost/signal.hpp>

namespace CamelotEditor
{
	class QtDynamicTabBar : public QWidget
	{
		Q_OBJECT

	public:
		QtDynamicTabBar(QWidget* parent);
		virtual ~QtDynamicTabBar() { }

		void addTab(const QString& name);
		void insertTab(UINT32 idx, const QString& name);
		void removeTab(UINT32 idx);

		vector<QPolygon>::type getDropLocations();

		void enterEvent(QEvent *e);
		void leaveEvent(QEvent *e);
		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void paintEvent(QPaintEvent *event);

		boost::signal<void(UINT32)> onTabSelected;
		boost::signal<void(UINT32)> onTabDraggedOff;
	private:
		QHBoxLayout* mHLayout;
		vector<QtTabButton*>::type mTabs;
		bool dbg;

		void setupUi();
		void updateTabs();

		void tabSelected(UINT32 idx);
		void tabDraggedOff(UINT32 idx);
	};
}