#include "BsGUIWindowFrameWidget.h"
#include "BsGUIWindowFrameWidgetRTTI.h"
#include "BsGUIPanel.h"
#include "BsCGUIWidget.h"
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

	WindowFrameWidget::WindowFrameWidget(const HSceneObject& parent, bool allowResize, const CameraPtr& camera, RenderWindow* parentWindow, const HGUISkin& skin)
		:CGUIWidget(parent, camera), mWindowFramePanel(nullptr), mParentWindow(parentWindow), mAllowResize(allowResize)
	{
		setSkin(skin);

		GUIPanel* backgroundPanel = getPanel()->addNewElement<GUIPanel>(500);
		backgroundPanel->addElement(GUITexture::create(GUIImageScaleMode::RepeatToFit, 
			GUIOptions(GUIOption::flexibleWidth(), GUIOption::flexibleHeight()), "WindowBackground"));

		mWindowFramePanel = getPanel()->addNewElement<GUIPanel>(499);

		mWindowFrame = GUIWindowFrame::create("WindowFrame");
		mWindowFramePanel->addElement(mWindowFrame);

		refreshNonClientAreas();
	}

	WindowFrameWidget::~WindowFrameWidget()
	{

	}

	void WindowFrameWidget::update()
	{

	}

	void WindowFrameWidget::ownerWindowFocusChanged()
	{
		mWindowFrame->setFocused(mParentWindow->getProperties().hasFocus());

		CGUIWidget::ownerWindowFocusChanged();
	}

	void WindowFrameWidget::ownerTargetResized()
	{
		CGUIWidget::ownerTargetResized();

		refreshNonClientAreas();
	}

	void WindowFrameWidget::refreshNonClientAreas() const
	{
		if (!mAllowResize)
			return;

		INT32 x = 0;
		INT32 y = 0;

		UINT32 width = getTarget()->getWidth();
		UINT32 height = getTarget()->getHeight();

		Vector<NonClientResizeArea> nonClientAreas(8);

		nonClientAreas[0].type = NonClientAreaBorderType::TopLeft;
		nonClientAreas[0].area = Rect2I(x, y, 
			RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);

		nonClientAreas[1].type = NonClientAreaBorderType::Top;
		nonClientAreas[1].area = Rect2I(x + RESIZE_BORDER_WIDTH, y, 
			width - 2 * RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);

		nonClientAreas[2].type = NonClientAreaBorderType::TopRight;
		nonClientAreas[2].area = Rect2I(x + width - RESIZE_BORDER_WIDTH, y, 
			RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);

		nonClientAreas[3].type = NonClientAreaBorderType::Left;
		nonClientAreas[3].area = Rect2I(x, y + RESIZE_BORDER_WIDTH, 
			RESIZE_BORDER_WIDTH, height - 2 * RESIZE_BORDER_WIDTH);

		nonClientAreas[4].type = NonClientAreaBorderType::Right;
		nonClientAreas[4].area = Rect2I(x + width - RESIZE_BORDER_WIDTH, y + RESIZE_BORDER_WIDTH, 
			RESIZE_BORDER_WIDTH, height - 2 * RESIZE_BORDER_WIDTH);

		nonClientAreas[5].type = NonClientAreaBorderType::BottomLeft;
		nonClientAreas[5].area = Rect2I(x, y + height - RESIZE_BORDER_WIDTH, 
			RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);

		nonClientAreas[6].type = NonClientAreaBorderType::Bottom;
		nonClientAreas[6].area = Rect2I(x + RESIZE_BORDER_WIDTH, y + height - RESIZE_BORDER_WIDTH, 
			width - 2 * RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);

		nonClientAreas[7].type = NonClientAreaBorderType::BottomRight;
		nonClientAreas[7].area = Rect2I(x + width - RESIZE_BORDER_WIDTH, y + height - RESIZE_BORDER_WIDTH, 
			RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);

		Platform::setResizeNonClientAreas(*mParentWindow->getCore(), nonClientAreas);
	}

	RTTITypeBase* WindowFrameWidget::getRTTIStatic()
	{
		return WindowFrameWidgetRTTI::instance();
	}

	RTTITypeBase* WindowFrameWidget::getRTTI() const
	{
		return WindowFrameWidget::getRTTIStatic();
	}
}