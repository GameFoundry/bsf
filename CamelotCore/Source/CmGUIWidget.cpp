#include "CmGUIWidget.h"
#include "CmGUI.h"
#include "CmGUIManager.h"

namespace CamelotEngine
{
	GUIWidget::GUIWidget(GameObjectPtr parent)
		:Component(parent), mGUI(new GUI())
	{
		GUIManager::instance().registerWidget(this);
	}

	GUIWidget::~GUIWidget()
	{
		GUIManager::instance().unregisterWidget(this);

		delete mGUI;
	}

	void GUIWidget::update()
	{

	}
}