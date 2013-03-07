#include "CmQtDynamicTabBar.h"
#include "CmException.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>

namespace CamelotEditor
{
	QtDynamicTabBar::QtDynamicTabBar(QWidget* parent)
		:QWidget(parent), mHLayout(nullptr)
	{
		setupUi();
	}

	void QtDynamicTabBar::addTab(const QString& name)
	{
		QPushButton* newBtn = new QPushButton(this);
		newBtn->setMaximumWidth(100);
		newBtn->setText(name);

		mTabs.push_back(newBtn);

		updateTabs();
	}

	void QtDynamicTabBar::insertTab(UINT32 idx, const QString& name)
	{
		if(idx >= (UINT32)mTabs.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) +". Valid range: 0 .. " + toString((UINT32)mTabs.size()));

		QPushButton* newBtn = new QPushButton(this);
		newBtn->setMaximumWidth(100);
		newBtn->setText(name);

		mTabs.insert(mTabs.begin() + idx, newBtn);

		updateTabs();
	}

	void QtDynamicTabBar::removeTab(UINT32 idx)
	{
		if(idx >= (UINT32)mTabs.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) +". Valid range: 0 .. " + toString((UINT32)mTabs.size()));

		mTabs.erase(mTabs.begin() + idx);

		updateTabs();
	}

	void QtDynamicTabBar::setupUi()
	{
		mHLayout = new QHBoxLayout(this);
		mHLayout->setMargin(0);

		setLayout(mHLayout);
	}

	void QtDynamicTabBar::updateTabs()
	{
		QLayoutItem* item;
		while((item = mHLayout->takeAt(0)) != nullptr)
			mHLayout->removeItem(item);

		UINT32 idx = 0;
		for(auto iter = mTabs.begin(); iter != mTabs.end(); ++iter)
		{
			(*iter)->disconnect();
			/*connect(*iter, &QPushButton::clicked, std::bind(this, &QtDynamicTabBar::tabSelected, idx));*/
			mHLayout->addWidget(*iter);

			idx++;
		}

		mHLayout->addStretch(2);
	}

	void QtDynamicTabBar::tabSelected(UINT32 idx)
	{
		onTabSelected(idx);
	}
}