#include "BsDropDownWindow.h"
#include "BsSceneObject.h"
#include "BsGUIWidget.h"
#include "BsGUIPanel.h"
#include "BsGUITexture.h"
#include "BsGUIWindowFrame.h"
#include "BsDropDownAreaPlacement.h"
#include "BsBuiltinEditorResources.h"
#include "BsGUIDropDownHitBox.h"
#include "BsDropDownWindowManager.h"

namespace BansheeEngine
{
	DropDownWindow::DropDownWindow(const RenderWindowPtr& parent, Viewport* target, 
		const Vector2I& position, UINT32 width, UINT32 height)
		:mRootPanel(nullptr), mPosition(position), mWidth(width), mHeight(height), 
		mRenderWindow(parent), mHitBox(nullptr)
	{
		mSceneObject = SceneObject::create("EditorWindow");

		mGUI = mSceneObject->addComponent<GUIWidget>(target);

		mGUI->setDepth(0); // Needs to be in front of everything
		mGUI->setSkin(BuiltinEditorResources::instance().getSkin());

		mRootPanel = mGUI->getPanel()->addNewElement<GUIPanel>();
		
		GUIPanel* hitBoxPanel = mRootPanel->addNewElement<GUIPanel>(std::numeric_limits<INT16>::min());

		mHitBox = GUIDropDownHitBox::create(false);
		mHitBox->onFocusLost.connect(std::bind(&DropDownWindow::dropDownFocusLost, this));
		mHitBox->setFocus(true);
		hitBoxPanel->addElement(mHitBox);

		GUIPanel* captureHitBoxPanel = mGUI->getPanel()->addNewElement<GUIPanel>(std::numeric_limits<INT16>::max());
		GUIDropDownHitBox* captureHitBox = GUIDropDownHitBox::create(true);
		captureHitBox->setBounds(Rect2I(0, 0, target->getWidth(), target->getHeight()));
		captureHitBoxPanel->addElement(captureHitBox);

		setSize(width, height);

		GUIPanel* backgroundPanel = mRootPanel->addNewElement<GUIPanel>(500);
		backgroundPanel->addElement(GUITexture::create(GUIImageScaleMode::RepeatToFit,
			GUIOptions(GUIOption::flexibleWidth(), GUIOption::flexibleHeight()), "WindowBackground"));

		GUIPanel* windowFramePanel = mRootPanel->addNewElement<GUIPanel>(499);

		GUIWindowFrame* windowFrame = GUIWindowFrame::create("WindowFrame");
		windowFramePanel->addElement(windowFrame);

		mContents = mRootPanel->addNewElement<GUIPanel>();
		mContents->setPosition(1, 1);
		mContents->setWidth(width - 2);
		mContents->setHeight(height - 2);
	}

	DropDownWindow::~DropDownWindow()
	{
		mSceneObject->destroy();
	}

	Vector2I DropDownWindow::screenToWindowPos(const Vector2I& screenPos) const
	{
		Vector2I renderWindowPos = mRenderWindow->screenToWindowPos(screenPos);

		Vector2I contentsPos = renderWindowPos;
		Rect2I contentArea = mContents->getGlobalBounds();

		contentsPos.x -= contentArea.x;
		contentsPos.y -= contentArea.y;

		return contentsPos;
	}

	Vector2I DropDownWindow::windowToScreenPos(const Vector2I& windowPos) const
	{
		Vector2I contentsPos = windowPos;
		Rect2I contentArea = mContents->getGlobalBounds();

		contentsPos.x += contentArea.x;
		contentsPos.y += contentArea.y;

		return mRenderWindow->windowToScreenPos(contentsPos);
	}

	void DropDownWindow::setSize(UINT32 width, UINT32 height)
	{
		Rect2I availableBounds(0, 0, mGUI->getTarget()->getWidth(), mGUI->getTarget()->getHeight());
		DropDownAreaPlacement dropDownPlacement = DropDownAreaPlacement::aroundPosition(mPosition);

		DropDownAreaPlacement::HorzDir horzDir;
		DropDownAreaPlacement::VertDir vertDir;
		Rect2I placementBounds = dropDownPlacement.getOptimalBounds(width, height, availableBounds, horzDir, vertDir);

		mRootPanel->setPosition(placementBounds.x, placementBounds.y);
		mRootPanel->setWidth(width);
		mRootPanel->setHeight(height);

		mHitBox->setBounds(Rect2I(placementBounds.x, placementBounds.y, width, height));

		mWidth = width;
		mHeight = height;
	}

	void DropDownWindow::close()
	{
		DropDownWindowManager::instance().close();
	}

	void DropDownWindow::dropDownFocusLost()
	{
		close();
	}
}