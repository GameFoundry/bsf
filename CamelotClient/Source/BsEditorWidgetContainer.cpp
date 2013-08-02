#include "BsEditorWidgetContainer.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	EditorWidgetContainer::EditorWidgetContainer()
	{
		// TODO - This should somehow initialize a title bar that will be visible in the parent editor window or dock manager

		//GameObjectHandle<TabbedTitleBar> titleBar = so->addComponent<TabbedTitleBar>();
		//titleBar->setSkin(&EngineGUI::instance().getSkin());
		//titleBar->initialize(camera->getViewport().get(), mRenderWindow.get());
		//titleBar->setDepth(127);
	}

	EditorWidgetContainer::~EditorWidgetContainer()
	{

	}

	void EditorWidgetContainer::add(EditorWidget& widget)
	{
		// TODO
	}

	void EditorWidgetContainer::remove(EditorWidget& widget)
	{
		// TODO
	}

	void EditorWidgetContainer::move(EditorWidget& widget, CM::UINT32 newPosition)
	{
		// TODO
	}
}