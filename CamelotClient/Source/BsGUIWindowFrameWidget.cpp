#include "BsGUIWindowFrameWidget.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUITexture.h"
#include "BsGUIWindowFrame.h"
#include "BsEngineGUI.h"
#include "BsGUIMouseEvent.h"
#include "CmRenderWindow.h"
#include "CmMath.h"
#include "CmPlatform.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const UINT32 WindowFrameWidget::RESIZE_BORDER_WIDTH = 3;

	WindowFrameWidget::WindowFrameWidget(const HSceneObject& parent)
		:GUIWidget(parent), mWindowFrameArea(nullptr)
	{

	}

	WindowFrameWidget::~WindowFrameWidget()
	{

	}

	void WindowFrameWidget::initialize(CM::Viewport* target, CM::RenderWindow* ownerWindow)
	{
		GUIWidget::initialize(target, ownerWindow);

		GUIArea* backgroundArea = GUIArea::createStretchedXY(*this, 0, 0, 0, 0, 500);
		backgroundArea->getLayout().addElement(GUITexture::create(*this, GUILayoutOptions::expandableXY(), GUIImageScaleMode::RepeatToFit, getSkin()->getStyle("WindowBackground")));

		mWindowFrameArea = GUIArea::createStretchedXY(*this, 0, 0, 0, 0, 499);

		mWindowFrame = GUIWindowFrame::create(*this, getSkin()->getStyle("WindowFrame"));
		mWindowFrameArea->getLayout().addElement(mWindowFrame);

		refreshNonClientAreas();
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
		mWindowFrame->setFocused(getOwnerWindow()->hasFocus());

		GUIWidget::ownerWindowFocusChanged();
	}

	void WindowFrameWidget::ownerWindowResized()
	{
		GUIWidget::ownerWindowResized();

		refreshNonClientAreas();
	}

	void WindowFrameWidget::refreshNonClientAreas() const
	{
		INT32 x = mWindowFrameArea->x();
		INT32 y = mWindowFrameArea->y();

		UINT32 width = mWindowFrameArea->width();
		UINT32 height = mWindowFrameArea->height();

		CM::Vector<NonClientResizeArea>::type nonClientAreas(8);

		nonClientAreas[0].type = NonClientAreaBorderType::TopLeft;
		nonClientAreas[0].area = Rect(x, y, 
			RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);

		nonClientAreas[1].type = NonClientAreaBorderType::Top;
		nonClientAreas[1].area = Rect(x + RESIZE_BORDER_WIDTH, y, 
			width - 2 * RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);

		nonClientAreas[2].type = NonClientAreaBorderType::TopRight;
		nonClientAreas[2].area = Rect(x + width - RESIZE_BORDER_WIDTH, y, 
			RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);

		nonClientAreas[3].type = NonClientAreaBorderType::Left;
		nonClientAreas[3].area = Rect(x, y + RESIZE_BORDER_WIDTH, 
			RESIZE_BORDER_WIDTH, height - 2 * RESIZE_BORDER_WIDTH);

		nonClientAreas[4].type = NonClientAreaBorderType::Right;
		nonClientAreas[4].area = Rect(x + width - RESIZE_BORDER_WIDTH, y + RESIZE_BORDER_WIDTH, 
			RESIZE_BORDER_WIDTH, height - 2 * RESIZE_BORDER_WIDTH);

		nonClientAreas[5].type = NonClientAreaBorderType::BottomLeft;
		nonClientAreas[5].area = Rect(x, y + height - RESIZE_BORDER_WIDTH, 
			RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);

		nonClientAreas[6].type = NonClientAreaBorderType::Bottom;
		nonClientAreas[6].area = Rect(x + RESIZE_BORDER_WIDTH, y + height - RESIZE_BORDER_WIDTH, 
			width - 2 * RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);

		nonClientAreas[7].type = NonClientAreaBorderType::BottomRight;
		nonClientAreas[7].area = Rect(x + width - RESIZE_BORDER_WIDTH, y + height - RESIZE_BORDER_WIDTH, 
			RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);

		Platform::setResizeNonClientAreas(*getOwnerWindow(), nonClientAreas);
	}
}