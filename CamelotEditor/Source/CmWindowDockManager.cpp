#include "CmWindowDockManager.h"
#include "CmQtEditorWindow.h"
#include "CmDebug.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QLayout>
#include <QtWidgets/QSplitter>

namespace CamelotEditor
{
	WindowDockManager::WindowDockManager(QWidget* centralWidget, QtDockOverlayWidget* dockOverlayWidget)
		:mDockOverlayWidget(dockOverlayWidget), mCentralWidget(centralWidget), mCentralSplitter(nullptr), mLastDraggedWindow(nullptr)
	{ 
		QVBoxLayout* boxLayout = new QVBoxLayout();
		boxLayout->setMargin(0);
		boxLayout->setSpacing(0);
		mCentralWidget->setLayout(boxLayout);

		mCentralSplitter = new QSplitter(mCentralWidget);
		mCentralSplitter->setChildrenCollapsible(false);
		boxLayout->addWidget(mCentralSplitter);
	}

	void WindowDockManager::windowDragged(QtEditorWindow* window, const QPoint& mousePos)
	{
		assert(window != nullptr);

		if(mLastDraggedWindow != window)
		{
			mLastDragPosition = mousePos;
			mLastDraggedWindow = window;
		}

		if(!window->isDocked())
		{
			QWidget* dragOverWidget = getDockedWindowAtPosition(mousePos);
			if(dragOverWidget == nullptr && isPositionInDockArea(mousePos))
				dragOverWidget = mCentralWidget;

			if(dragOverWidget != nullptr)
			{
				WindowDragDropLocation dragLocation = getDropLocationAtPosition(dragOverWidget, mousePos);
				std::vector<QPolygon> dropLocations = getDropLocations(dragOverWidget);

				QPoint drawOffset = mCentralWidget->mapToGlobal(QPoint(0, 0)) - mDockOverlayWidget->mapToGlobal(QPoint(0, 0));

				mDockOverlayWidget->enableDropOverlay(dropLocations, drawOffset);
				mDockOverlayWidget->highlightDropLocation(dragLocation);
			}
			else
			{
				mDockOverlayWidget->highlightDropLocation(CM_WINDROP_NONE);
				mDockOverlayWidget->disableDropOverlay();
			}
		}
		else
		{
			QPoint diff = mLastDragPosition - mousePos;
			
			if(diff.manhattanLength() > 4)
			{
				undockWindow(window);
			}
		}
	}

	void WindowDockManager::windowReleased(QtEditorWindow* window, const QPoint& mousePos)
	{
		mDockOverlayWidget->highlightDropLocation(CM_WINDROP_NONE);
		mDockOverlayWidget->disableDropOverlay();

		if(!window->isDocked())
		{
			QtEditorWindow* windowUnderCursor = getDockedWindowAtPosition(mousePos);
			if(windowUnderCursor != nullptr)
			{
				WindowDragDropLocation dropLocation = getDropLocationAtPosition(windowUnderCursor, mousePos);
				dockWindow(window, windowUnderCursor, dropLocation);
			}
			else
			{
				if(isPositionInDockArea(mousePos))
				{
					WindowDragDropLocation dropLocation = gWindowDockManager().getDropLocationAtPosition(mCentralWidget, mousePos);
					gWindowDockManager().dockWindow(window, nullptr, dropLocation);
				}
			}
		}
	}

	void WindowDockManager::windowClosed(QtEditorWindow* window)
	{
		if(window->isDocked())
			undockWindow(window);
	}

	QtEditorWindow* WindowDockManager::getDockedWindowAtPosition(const QPoint& globalPos)
	{
		QtEditorWindow* foundWindow = nullptr;
		for(auto iter = mDockedWindows.begin(); iter != mDockedWindows.end(); ++iter)
		{
			QtEditorWindow* curWindow = *iter;
			QPoint globalWidgetPos = curWindow->mapToGlobal(QPoint(0, 0));

			QRect widgetRect(globalWidgetPos, curWindow->geometry().size());

			if(widgetRect.contains(globalPos))
			{
				foundWindow = curWindow;
				break;
			}
		}

		return foundWindow;
	}

	bool WindowDockManager::isPositionInDockArea(const QPoint& globalPos)
	{
		QPoint globalWidgetPos = mCentralWidget->mapToGlobal(QPoint(0, 0));
		QRect widgetRect(globalWidgetPos, mCentralWidget->geometry().size());

		return widgetRect.contains(globalPos);
	}

	WindowDragDropLocation WindowDockManager::getDropLocationAtPosition(const QWidget* widget, const QPoint& globalPos)
	{
		assert(widget != nullptr);

		QPoint localPos = mCentralWidget->mapFromGlobal(globalPos);

		std::vector<QPolygon> dragLocations = getDropLocations(widget);
		int idx = 0;
		for(auto iter = dragLocations.begin(); iter != dragLocations.end(); ++iter)
		{
			if(iter->containsPoint(localPos, Qt::OddEvenFill))
				return (WindowDragDropLocation)idx;

			++idx;
		}

		return CM_WINDROP_NONE;
	}

	void WindowDockManager::dockWindow(QtEditorWindow* windowToDock, QtEditorWindow* dockAtWidget, WindowDragDropLocation dockAtPosition)
	{
		assert(windowToDock != nullptr);

		auto findIter = std::find(mDockedWindows.begin(), mDockedWindows.end(), windowToDock);
		assert(findIter == mDockedWindows.end());

		if(dockAtPosition == CM_WINDROP_NONE)
			return;

		if(dockAtWidget == nullptr)
		{
			mCentralSplitter->addWidget(windowToDock);

			windowToDock->dock();
		}
		else
		{
			QSplitter* parentSplitter = dynamic_cast<QSplitter*>(dockAtWidget->parentWidget());
			if(parentSplitter == nullptr)
			{
				LOGWRN("Trying to dock a window to a widget that doesn't have a parent splitter.");
				return;
			}

			if(dockAtPosition == CM_WINDROP_CENTER)
			{
				CM_ASSERT(false); // TODO - Not implemented
			}
			else
			{
				if(parentSplitter->orientation() == Qt::Horizontal)
				{
					int idxDockAt = parentSplitter->indexOf(dockAtWidget);
					if(dockAtPosition == CM_WINDROP_LEFT)
						parentSplitter->insertWidget(idxDockAt, windowToDock);
					else if(dockAtPosition == CM_WINDROP_RIGHT)
						parentSplitter->insertWidget(idxDockAt + 1, windowToDock);
					else // Top or bottom
					{
						QSplitter* newSplitter = new QSplitter();
						newSplitter->setOrientation(Qt::Vertical);
						newSplitter->setChildrenCollapsible(false);
						
						if(dockAtPosition == CM_WINDROP_TOP)
						{
							newSplitter->addWidget(windowToDock);
							newSplitter->addWidget(dockAtWidget);
						}
						else
						{
							newSplitter->addWidget(dockAtWidget);
							newSplitter->addWidget(windowToDock);
						}

						parentSplitter->insertWidget(idxDockAt, newSplitter);
					}
				}
				else
				{
					int idxDockAt = parentSplitter->indexOf(dockAtWidget);
					if(dockAtPosition == CM_WINDROP_TOP)
						parentSplitter->insertWidget(idxDockAt, windowToDock);
					else if(dockAtPosition == CM_WINDROP_BOTTOM)
						parentSplitter->insertWidget(idxDockAt + 1, windowToDock);
					else // Left or right
					{
						QSplitter* newSplitter = new QSplitter();
						newSplitter->setOrientation(Qt::Horizontal);
						newSplitter->setChildrenCollapsible(false);

						if(dockAtPosition == CM_WINDROP_LEFT)
						{
							newSplitter->addWidget(windowToDock);
							newSplitter->addWidget(dockAtWidget);
						}
						else
						{
							newSplitter->addWidget(dockAtWidget);
							newSplitter->addWidget(windowToDock);
						}

						parentSplitter->insertWidget(idxDockAt, newSplitter);
					}
				}
			}

			windowToDock->dock();
		}

		mDockedWindows.push_back(windowToDock);
	}

	void WindowDockManager::undockWindow(QtEditorWindow* windowToUndock)
	{
		CM_ASSERT(windowToUndock != nullptr);

		QSplitter* parentSplitter = dynamic_cast<QSplitter*>(windowToUndock->parentWidget());
		if(parentSplitter == nullptr)
		{
			LOGWRN("Trying to dock a window to a widget that doesn't have a parent splitter.");
			return;
		}

		windowToUndock->setParent(mCentralWidget);
		windowToUndock->undock();
		windowToUndock->show();

		// Check if there is just one widget in splitter, so there's no need for a splitter at all
		if(parentSplitter != mCentralSplitter && parentSplitter->count() == 1) 
		{
			QSplitter* topParentSplitter = dynamic_cast<QSplitter*>(parentSplitter->parentWidget());
			if(topParentSplitter == nullptr)
			{
				CM_EXCEPT(InternalErrorException, "Splitter is not root splitter, but doesn't have a splitter parent.");
			}

			QWidget* remainingWidget = parentSplitter->widget(0);

			int splitterIdx = topParentSplitter->indexOf(parentSplitter);
			topParentSplitter->insertWidget(splitterIdx, remainingWidget);

			delete parentSplitter;
		}

		auto findIter = find(mDockedWindows.begin(), mDockedWindows.end(), windowToUndock);
		CM_ASSERT(findIter != mDockedWindows.end());

		mDockedWindows.erase(findIter);
	}

	std::vector<QPolygon> WindowDockManager::getDropLocations(const QWidget* widget)
	{
		QPoint topLeft = widget->mapToGlobal(QPoint(0, 0));
		topLeft = mCentralWidget->mapFromGlobal(topLeft);

		QRect largeRect(topLeft.x() + 1, topLeft.y() + 1, widget->width() - 2, widget->height() - 2);

		int sizeOffset = widget->width() < widget->height() ? (widget->width() / 10) : (widget->height() / 10);
		QRect smallRect(topLeft.x() + sizeOffset, topLeft.y() + sizeOffset, widget->width() - 2 * sizeOffset, widget->height() - 2 * sizeOffset);

		QPolygon left(4); 
		left[0] = largeRect.topLeft();
		left[1] = largeRect.bottomLeft();
		left[2] = smallRect.bottomLeft();
		left[3] = smallRect.topLeft();

		QPolygon right(4);
		right[0] = largeRect.topRight();
		right[1] = largeRect.bottomRight();
		right[2] = smallRect.bottomRight();
		right[3] = smallRect.topRight();

		QPolygon top(4);
		top[0] = largeRect.topLeft();
		top[1] = largeRect.topRight();
		top[2] = smallRect.topRight();
		top[3] = smallRect.topLeft();

		QPolygon bottom(4);
		bottom[0] = largeRect.bottomRight();
		bottom[1] = largeRect.bottomLeft();
		bottom[2] = smallRect.bottomLeft();
		bottom[3] = smallRect.bottomRight();

		QPolygon center(4);
		center[0] = smallRect.topLeft();
		center[1] = smallRect.topRight();
		center[2] = smallRect.bottomRight();
		center[3] = smallRect.bottomLeft();

		std::vector<QPolygon> dragLocations;
		dragLocations.push_back(left);
		dragLocations.push_back(right);
		dragLocations.push_back(top);
		dragLocations.push_back(bottom);
		dragLocations.push_back(center);

		return dragLocations;
	}

	WindowDockManager& gWindowDockManager()
	{
		return WindowDockManager::instance();
	}
}