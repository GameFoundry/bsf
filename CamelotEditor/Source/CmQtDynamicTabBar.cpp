#include "CmQtDynamicTabBar.h"
#include "CmException.h"
#include "CmQtTabButton.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtGui/QPainter>

namespace CamelotEditor
{
	QtDynamicTabBar::QtDynamicTabBar(QWidget* parent)
		:QWidget(parent), mHLayout(nullptr), dbg(false)
	{
		setupUi();
	}

	void QtDynamicTabBar::addTab(const QString& name)
	{
		QtTabButton* newBtn = new QtTabButton(this);
		newBtn->setMaximumWidth(100);
		newBtn->setText(name);

		mTabs.push_back(newBtn);

		updateTabs();
	}

	void QtDynamicTabBar::insertTab(UINT32 idx, const QString& name)
	{
		if(idx > (UINT32)mTabs.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) +". Valid range: 0 .. " + toString((UINT32)mTabs.size()));

		QtTabButton* newBtn = new QtTabButton(this);
		newBtn->setMaximumWidth(100);
		newBtn->setText(name);

		mTabs.insert(mTabs.begin() + idx, newBtn);

		updateTabs();
	}

	void QtDynamicTabBar::removeTab(UINT32 idx)
	{
		if(idx >= (UINT32)mTabs.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) +". Valid range: 0 .. " + toString((UINT32)mTabs.size()));

		auto iter = mTabs.begin() + idx;
		delete *iter;

		mTabs.erase(iter);

		updateTabs();
	}

	vector<QPolygon>::type QtDynamicTabBar::getDropLocations()
	{
		QPoint topLeft = mapToGlobal(QPoint(0, 0));
		int curX = topLeft.x();
		int endX = topLeft.x() + width();
		int top = topLeft.y();
		int bottom = topLeft.y() + height();

		QRect rect(topLeft.x(), topLeft.y(), width(), height());

		vector<QPolygon>::type polygons;
		QPolygon polygon(4);

		polygon[0] = QPoint(curX, top);
		polygon[1] = QPoint(curX, bottom);

		for(auto iter = mTabs.begin(); iter != mTabs.end(); ++iter)
		{
			QPoint tabTopLeft = (*iter)->mapToGlobal(QPoint(0, 0));
			curX = tabTopLeft.x();

			polygon[2] = QPoint(curX, bottom);
			polygon[3] = QPoint(curX, top);
			polygons.push_back(polygon);

			polygon = QPolygon(4);
			curX = tabTopLeft.x() + (*iter)->width();
			polygon[0] = QPoint(curX, top);
			polygon[1] = QPoint(curX, bottom);
		}

		polygon[2] = QPoint(endX, bottom);
		polygon[3] = QPoint(endX, top);
		polygons.push_back(polygon);

		return polygons;
	}

	void QtDynamicTabBar::enterEvent(QEvent *e)
	{

	}

	void QtDynamicTabBar::leaveEvent(QEvent *e)
	{

	}

	void QtDynamicTabBar::mousePressEvent(QMouseEvent* event)
	{
		QWidget::mousePressEvent(event);
	}

	void QtDynamicTabBar::mouseReleaseEvent(QMouseEvent* event)
	{
		dbg = true;
		repaint();

		QWidget::mouseReleaseEvent(event);
	}

	void QtDynamicTabBar::mouseMoveEvent(QMouseEvent* event)
	{
		dbg = false;
		repaint();

		QWidget::mouseMoveEvent(event);
	}

	void QtDynamicTabBar::paintEvent(QPaintEvent *event)
	{
		if(dbg)
		{
			QPainter painter(this);
			painter.setRenderHint(QPainter::Antialiasing);
			painter.setPen(QColor(208, 208, 208));
			painter.setClipping(false);

			painter.fillRect(0, 0, 100, 100, QColor(190, 190, 190, 128));
		}
	}

	void QtDynamicTabBar::setupUi()
	{
		setMouseTracking(true);

		mHLayout = new QHBoxLayout(this);
		mHLayout->setMargin(0);

		setLayout(mHLayout);
	}

	void QtDynamicTabBar::updateTabs()
	{
		QLayoutItem* item;
		while((item = mHLayout->takeAt(0)) != nullptr)
			mHLayout->removeItem(item);

		mHLayout->addSpacing(10);

		UINT32 idx = 0;
		for(auto iter = mTabs.begin(); iter != mTabs.end(); ++iter)
		{
			(*iter)->onSelected.disconnect_all_slots();
			(*iter)->onSelected.connect(boost::bind(&QtDynamicTabBar::tabSelected, this, idx));

			(*iter)->onDragged.disconnect_all_slots();
			(*iter)->onDragged.connect(boost::bind(&QtDynamicTabBar::tabDraggedOff, this, idx));

			mHLayout->addWidget(*iter);

			idx++;
		}

		mHLayout->addStretch(2);
	}

	void QtDynamicTabBar::tabSelected(UINT32 idx)
	{
		onTabSelected(idx);
	}

	void QtDynamicTabBar::tabDraggedOff(UINT32 idx)
	{
		onTabDraggedOff(idx);
	}
}