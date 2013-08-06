#include "BsGUIToggleGroup.h"
#include "BsGUIToggle.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIToggleGroup::~GUIToggleGroup()
	{
		for(auto& button : mButtons)
		{
			button->_setToggleGroup(nullptr);
		}
	}

	void GUIToggleGroup::initialize(const std::shared_ptr<GUIToggleGroup>& sharedPtr)
	{
		mThis = sharedPtr;
	}

	void GUIToggleGroup::add(GUIToggle* toggle)
	{
		auto iterFind = std::find(begin(mButtons), end(mButtons), toggle);
		if(iterFind != end(mButtons))
			return;

		mButtons.push_back(toggle);
		toggle->_setToggleGroup(mThis.lock());
	}

	void GUIToggleGroup::remove(GUIToggle* toggle)
	{
		auto iterFind = std::find(begin(mButtons), end(mButtons), toggle);
		if(iterFind == end(mButtons))
			return;

		(*iterFind)->_setToggleGroup(nullptr);
		mButtons.erase(iterFind);
	}
}