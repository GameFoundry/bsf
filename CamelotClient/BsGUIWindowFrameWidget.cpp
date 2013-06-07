#include "BsGUIWindowFrameWidget.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUITexture.h"
#include "BsGUIWindowFrame.h"
#include "BsEngineGUI.h"
#include "BsGUIMouseEvent.h"
#include "CmRenderWindow.h"
#include "CmMath.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
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

		GUIArea* backgroundArea = GUIArea::create(*this, 0, 0, 0, 0, 500);
		backgroundArea->getLayout().addElement(GUITexture::create(*this, GUILayoutOptions::expandableXY(), GUIImageScaleMode::RepeatToFit, getSkin()->getStyle("WindowBackground")));

		mWindowFrameArea = GUIArea::create(*this, 0, 0, 0, 0, 499);

		mWindowFrame = GUIWindowFrame::create(*this, getSkin()->getStyle("WindowFrame"));
		mWindowFrameArea->getLayout().addElement(mWindowFrame);
	}

	void WindowFrameWidget::update()
	{

	}

	bool WindowFrameWidget::_mouseEvent(GUIElement* element, const GUIMouseEvent& ev)
	{
		return GUIWidget::_mouseEvent(element, ev);
	}

	void WindowFrameWidget::ownerWindowFocusChanged(CM::RenderWindow* window)
	{
		mWindowFrame->setFocused(window->hasFocus());
	}
}