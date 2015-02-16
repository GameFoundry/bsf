#include "BsModalWindow.h"
#include "BsEditorWindowManager.h"
#include "BsRenderWindow.h"
#include "BsPlatform.h"
#include "BsGUIArea.h"
#include "BsGUILayoutX.h"
#include "BsGUISpace.h"
#include "BsGUIButton.h"
#include "BsGUITexture.h"
#include "BsGUILabel.h"

namespace BansheeEngine
{
	ModalWindow::ModalWindow(const HString& title, bool hasCloseButton)
		:EditorWindowBase(true), mTitleBarArea(nullptr), mTitleBarBgArea(nullptr), 
		mCloseButton(nullptr), mTitleBarBg(nullptr), mTitle(nullptr)
	{
		mTitleBarBgArea = GUIArea::createStretchedXY(*mGUI, 1, 1, 1, 1, 1);
		mTitleBarArea = GUIArea::createStretchedXY(*mGUI, 1, 1, 1, 1, 0);

		mTitleBarBg = GUITexture::create(GUIOptions(GUIOption::flexibleWidth()), "TitleBarBackground");
		mTitle = GUILabel::create(title);

		GUILayout& bgLayout = mTitleBarBgArea->getLayout().addLayoutX();
		bgLayout.addElement(mTitleBarBg);
		bgLayout.addFlexibleSpace();

		GUILayout& contentLayout = mTitleBarArea->getLayout().addLayoutX();
		contentLayout.addFlexibleSpace();
		contentLayout.addElement(mTitle);
		contentLayout.addFlexibleSpace();

		if (hasCloseButton)
		{
			mCloseButton = GUIButton::create(HString(L""), "WinCloseBtn");

			GUILayout& contentLayout = mTitleBarArea->getLayout().addLayoutX();
			contentLayout.addFlexibleSpace();
			contentLayout.addElement(mCloseButton);
			
			mCloseButton->onClick.connect(std::bind(&ModalWindow::close, this));
		}

		mTitleBarArea->getLayout().addFlexibleSpace();
		
		updateSize();
	}

	ModalWindow::~ModalWindow()
	{

	}

	void ModalWindow::update()
	{

	}

	void ModalWindow::close()
	{
		bs_delete(this);
	}

	void ModalWindow::setTitle(const HString& title)
	{
		mTitle->setContent(GUIContent(title));
	}

	void ModalWindow::resized()
	{
		EditorWindowBase::resized();

		updateSize();
	}

	void ModalWindow::updateSize()
	{
		Vector<Rect2I> captionAreas;
		captionAreas.push_back(Rect2I(1, 1, getWidth() - 2, getTitleBarHeight()));

		Platform::setCaptionNonClientAreas(*mRenderWindow->getCore().get(), captionAreas);
	}

	Rect2I ModalWindow::getContentArea() const
	{
		return Rect2I(1, 1 + getTitleBarHeight(), getWidth() - 2, getHeight() - getTitleBarHeight() - 2);
	}

	UINT32 ModalWindow::getTitleBarHeight() const
	{
		return mTitleBarBg->getBounds().height;
	}
}