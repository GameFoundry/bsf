#include "CmGUIWidget.h"
#include "CmGUIManager.h"
#include "CmGUISkin.h"
#include "CmGUILabel.h"

namespace CamelotEngine
{
	GUISkin GUIWidget::DefaultSkin;

	GUIWidget::GUIWidget(GameObjectPtr parent)
		:Component(parent), mSkin(nullptr), mMeshGroupID(0)
	{
		GUIManager::instance().registerWidget(this);
	}

	GUIWidget::~GUIWidget()
	{
		GUIManager::instance().unregisterWidget(this);

		for(auto& elem : mElements)
		{
			delete elem;
		}

		mElements.clear();
	}

	GUILabel* GUIWidget::addLabel(const String& text)
	{
		GUILabel* label = new GUILabel(this, text, getGUISkin());
		mElements.push_back(label);

		return label;
	}

	const GUISkin* GUIWidget::getGUISkin() const
	{
		if(mSkin != nullptr)
			return mSkin;
		else
			return &DefaultSkin;
	}

	void GUIWidget::update()
	{

	}
}