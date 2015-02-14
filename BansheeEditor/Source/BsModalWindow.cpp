#include "BsModalWindow.h"
#include "BsEditorWindowManager.h"
#include "BsRenderWindow.h"
#include "BsPlatform.h"

namespace BansheeEngine
{
	ModalWindow::ModalWindow()
		:EditorWindowBase(true)
	{
		updateSize();
	}

	ModalWindow::~ModalWindow()
	{

	}

	void ModalWindow::update()
	{

	}

	void ModalWindow::resized()
	{
		EditorWindowBase::resized();

		updateSize();
	}

	void ModalWindow::updateSize()
	{
		Vector<Rect2I> captionAreas;
		captionAreas.push_back(Rect2I(0, 0, getWidth(), 20));

		Platform::setCaptionNonClientAreas(*mRenderWindow->getCore().get(), captionAreas);
	}

	ModalWindow* ModalWindow::create()
	{
		return EditorWindowManager::instance().createModal();
	}
}