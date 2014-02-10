#include "BsInputConfiguration.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	Map<String, UINT32>::type VirtualButton::UniqueButtonIds;
	UINT32 VirtualButton::NextButtonId = 0;

	VIRTUAL_BUTTON_DESC::VIRTUAL_BUTTON_DESC()
		:buttonCode(BC_0), modifiers(VButtonModifier::None), repeatable(false)
	{ }

	VIRTUAL_BUTTON_DESC::VIRTUAL_BUTTON_DESC(ButtonCode buttonCode, VButtonModifier modifiers, bool repeatable)
		:buttonCode(buttonCode), modifiers(modifiers), repeatable(repeatable)
	{ }

	VirtualButton::VirtualButton()
		:buttonIdentifier(0)
	{ }

	VirtualButton::VirtualButton(const CM::String& name)
	{
		auto findIter = UniqueButtonIds.find(name);

		if(findIter != UniqueButtonIds.end())
			buttonIdentifier = findIter->second;
		else
		{
			buttonIdentifier = NextButtonId;
			UniqueButtonIds[name] = NextButtonId++;
		}
	}

	InputConfiguration::InputConfiguration()
		:mRepeatInterval(1.0f)
	{ }

	void InputConfiguration::registerButton(const CM::String& name, CM::ButtonCode buttonCode, VButtonModifier modifiers, bool repeatable)
	{
		Vector<VirtualButtonData>::type& btnData = mButtons[buttonCode & 0x0000FFFF];

		INT32 idx = -1;
		for(UINT32 i = 0; i < (UINT32)btnData.size(); i++)
		{
			if(btnData[i].desc.modifiers == modifiers)
			{
				idx = (INT32)i;
				break;
			}
		}

		if(idx == -1)
		{
			idx = (INT32)btnData.size();
			btnData.push_back(VirtualButtonData());
		}

		VirtualButtonData& btn = btnData[idx];
		btn.name = name;
		btn.desc = VIRTUAL_BUTTON_DESC(buttonCode, modifiers, repeatable);
		btn.button = VirtualButton(name);
	}

	void InputConfiguration::unregisterButton(const CM::String& name)
	{
		Vector<UINT32>::type toRemove;

		for(UINT32 i = 0; i < BC_Count; i++)
		{
			for(UINT32 j = 0; j < (UINT32)mButtons[i].size(); j++)
			{
				if(mButtons[i][j].name == name)
					toRemove.push_back(j);
			}

			UINT32 numRemoved = 0;
			for(auto& toRemoveIdx : toRemove)
			{
				mButtons[i].erase(mButtons[i].begin() + toRemoveIdx - numRemoved);

				numRemoved++;
			}

			toRemove.clear();
		}
	}

	bool InputConfiguration::getButton(CM::ButtonCode code, UINT32 modifiers, VirtualButton& btn, VIRTUAL_BUTTON_DESC& btnDesc) const
	{
		const Vector<VirtualButtonData>::type& btnData = mButtons[code & 0x0000FFFF];

		for(UINT32 i = 0; i < (UINT32)btnData.size(); i++)
		{
			if((((UINT32)btnData[i].desc.modifiers) & modifiers) == ((UINT32)btnData[i].desc.modifiers))
			{
				btn = btnData[i].button;
				btnDesc = btnData[i].desc;
				return true;
			}
		}

		return false;
	}
}