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

		boost::signal<void(UINT32)> onTabSelected;

	private:
		QHBoxLayout* mHLayout;
		vector<QPushButton*>::type mTabs;

		void setupUi();
		void updateTabs();

		void tabSelected(UINT32 idx);
	};
}