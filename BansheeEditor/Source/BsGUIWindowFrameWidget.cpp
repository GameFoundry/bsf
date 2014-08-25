#include "BsGUIWindowFrameWidget.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUITexture.h"
#include "BsGUIWindowFrame.h"
#include "BsBuiltinResources.h"
#include "BsGUIMouseEvent.h"
#include "BsRenderWindow.h"
#include "BsMath.h"
#include "BsPlatform.h"

namespace BansheeEngine
{
	const UINT32 WindowFrameWidget::RESIZE_BORDER_WIDTH = 3;

	WindowFrameWidget::WindowFrameWidget(const HSceneObject& parent, Viewport* target, RenderWindow* parentWindow, const GUISkin& skin)
		:GUIWidget(parent, target), mWindowFrameArea(nullptr), mParentWindow(parentWindow)
	{
		setSkin(skin);

		GUIArea* backgroundArea = GUIArea::createStretchedXY(*this, 0, 0, 0, 0, 500);
		backgroundArea->getLayout().addElement(GUITexture::create(GUIImageScaleMode::RepeatToFit, 
			GUIOptions(GUIOption::flexibleWidth(), GUIOption::flexibleHeight()), "WindowBackground"));

		mWindowFrameArea = GUIArea::createStretchedXY(*this, 0, 0, 0, 0, 499);

		mWindowFrame = GUIWindowFrame::create("WindowFrame");
		mWindowFrameArea->getLayout().addElement(mWindowFrame);

		refreshNonClientAreas();
	}

	WindowFrameWidget::~WindowFrameWidget()
	{

	}

	void WindowFrameWidget::update()
	{

	}

	bool WindowFrameWidget::_mouseEvent(GUIElement* element, const GUIMouseEvent& ev)
	{
		return GUIWidget::_mouseEvent(element, ev);
	}

	void WindowFrameWidget::ownerWindowFocusChanged()
	{
		mWindowFrame->setFocused(mParentWindow->getProperties().hasFocus());

		GUIWidget::ownerWindowFocusChanged();
	}

	void WindowFrameWidget::ownerTargetResized()
	{
		GUIWidget::ownerTargetResized();

		refreshNonClientAreas();
	}

	void WindowFrameWidget::refreshNonClientAreas() const
	{
		INT32 x = mWindowFrameArea->x();
		INT32 y = mWindowFrameArea->y();

		UINT32 width = mWindowFrameArea->width();
		UINT32 height = mWindowFrameArea->height();

		Vector<NonClientResizeArea> nonClientAreas(8);

		nonClientAreas[0].type = NonClientAreaBorderType::TopLeft;
		nonClientAreas[0].area = RectI(x, y, 
			RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);

		nonClientAreas[1].type = NonClientAreaBorderType::Top;
		nonClientAreas[1].area = RectI(x + RESIZE_BORDER_WIDTH, y, 
			width - 2 * RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);

		nonClientAreas[2].type = NonClientAreaBorderType::TopRight;
		nonClientAreas[2].area = RectI(x + width - RESIZE_BORDER_WIDTH, y, 
			RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);

		nonClientAreas[3].type = NonClientAreaBorderType::Left;
		nonClientAreas[3].area = RectI(x, y + RESIZE_BORDER_WIDTH, 
			RESIZE_BORDER_WIDTH, height - 2 * RESIZE_BORDER_WIDTH);

		nonClientAreas[4].type = NonClientAreaBorderType::Right;
		nonClientAreas[4].area = RectI(x + width - RESIZE_BORDER_WIDTH, y + RESIZE_BORDER_WIDTH, 
			RESIZE_BORDER_WIDTH, height - 2 * RESIZE_BORDER_WIDTH);

		nonClientAreas[5].type = NonClientAreaBorderType::BottomLeft;
		nonClientAreas[5].area = RectI(x, y + height - RESIZE_BORDER_WIDTH, 
			RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);

		nonClientAreas[6].type = NonClientAreaBorderType::Bottom;
		nonClientAreas[6].area = RectI(x + RESIZE_BORDER_WIDTH, y + height - RESIZE_BORDER_WIDTH, 
			width - 2 * RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);

		nonClientAreas[7].type = NonClientAreaBorderType::BottomRight;
		nonClientAreas[7].area = RectI(x + width - RESIZE_BORDER_WIDTH, y + height - RESIZE_BORDER_WIDTH, 
			RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);

		Platform::setResizeNonClientAreas(*mParentWindow, nonClientAreas);
	}
}