#include "BsVirtualInput.h"
#include "BsInput.h"
#include "BsTime.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	VirtualInput::VirtualInput()
		:mActiveModifiers((UINT32)VButtonModifier::None)
	{
		mInputConfiguration = createConfiguration();

		Input::instance().onButtonDown.connect(std::bind(&VirtualInput::buttonDown, this, _1));
		Input::instance().onButtonUp.connect(std::bind(&VirtualInput::buttonUp, this, _1));
	}

	std::shared_ptr<InputConfiguration> VirtualInput::createConfiguration()
	{
		return bs_shared_ptr<InputConfiguration>();
	}

	void VirtualInput::setConfiguration(const std::shared_ptr<InputConfiguration>& input)
	{
		mInputConfiguration = input;

		mCachedStates.clear(); // Note: Technically this is slightly wrong as it will
		// "forget" any buttons currently held down, but shouldn't matter much in practice.
	}

	bool VirtualInput::isButtonDown(const VirtualButton& button) const
	{
		auto iterFind = mCachedStates.find(button.buttonIdentifier);

		if(iterFind != mCachedStates.end())
			return iterFind->second.state == ButtonState::ToggledOn;
		
		return false;
	}

	bool VirtualInput::isButtonUp(const VirtualButton& button) const
	{
		auto iterFind = mCachedStates.find(button.buttonIdentifier);

		if(iterFind != mCachedStates.end())
			return iterFind->second.state == ButtonState::ToggledOff;

		return false;
	}

	bool VirtualInput::isButtonHeld(const VirtualButton& button) const
	{
		auto iterFind = mCachedStates.find(button.buttonIdentifier);

		if(iterFind != mCachedStates.end())
			return iterFind->second.state == ButtonState::On || iterFind->second.state == ButtonState::ToggledOn;

		return false;
	}

	void VirtualInput::update()
	{
		for(auto& state : mCachedStates)
		{
			if(state.second.state == ButtonState::ToggledOff)
				state.second.state = ButtonState::Off;
			else if(state.second.state == ButtonState::ToggledOn)
				state.second.state = ButtonState::On;
		}

		bool hasEvents = true;
		UINT64 repeatInternal = mInputConfiguration->getRepeatInterval();
		UINT64 currentTime = gTime().getTimeMs();

		// Trigger all events
		while(hasEvents)
		{
			while(!mEvents.empty())
			{
				VirtualButtonEvent& event = mEvents.front();

				if(event.state == ButtonState::On)
				{
					if(!onButtonDown.empty())
						onButtonDown(event.button);
				}
				else if(event.state == ButtonState::Off)
				{
					if(!onButtonUp.empty())
						onButtonUp(event.button);
				}

				mEvents.pop();
			}

			// Queue up any repeatable events
			hasEvents = false;
			
			for(auto& state : mCachedStates)
			{
				if(state.second.state != ButtonState::On)
					continue;

				if(!state.second.allowRepeat)
					continue;

				UINT64 diff = currentTime - state.second.timestamp;
				if(diff >= repeatInternal)
				{
					state.second.timestamp += repeatInternal;

					VirtualButtonEvent event;
					event.button = state.second.button;
					event.state = ButtonState::On;

					mEvents.push(event);
					hasEvents = true;
				}				
			}
		}
	}

	void VirtualInput::buttonDown(const ButtonEvent& event)
	{
		if(event.buttonCode == BC_LSHIFT || event.buttonCode == BC_RSHIFT)
			mActiveModifiers |= (UINT32)VButtonModifier::Shift;
		else if(event.buttonCode == BC_LCONTROL || event.buttonCode == BC_RCONTROL)
			mActiveModifiers |= (UINT32)VButtonModifier::Ctrl;
		else if(event.buttonCode == BC_LMENU || event.buttonCode == BC_RMENU)
			mActiveModifiers |= (UINT32)VButtonModifier::Alt;
		else
		{
			VirtualButton btn;
			VIRTUAL_BUTTON_DESC btnDesc;
			if(mInputConfiguration->getButton(event.buttonCode, mActiveModifiers, btn, btnDesc))
			{
				ButtonData& data = mCachedStates[btn.buttonIdentifier];

				data.button = btn;
				data.state = ButtonState::ToggledOn;
				data.timestamp = event.timestamp;
				data.allowRepeat = btnDesc.repeatable;

				VirtualButtonEvent virtualEvent;
				virtualEvent.button = btn;
				virtualEvent.state = ButtonState::On;

				mEvents.push(virtualEvent);
			}
		}
	}

	void VirtualInput::buttonUp(const ButtonEvent& event)
	{
		if(event.buttonCode == BC_LSHIFT || event.buttonCode == BC_RSHIFT)
			mActiveModifiers &= ~(UINT32)VButtonModifier::Shift;
		else if(event.buttonCode == BC_LCONTROL || event.buttonCode == BC_RCONTROL)
			mActiveModifiers &= ~(UINT32)VButtonModifier::Ctrl;
		else if(event.buttonCode == BC_LMENU || event.buttonCode == BC_RMENU)
			mActiveModifiers &= ~(UINT32)VButtonModifier::Alt;
		else
		{
			VirtualButton btn;
			VIRTUAL_BUTTON_DESC btnDesc;
			if(mInputConfiguration->getButton(event.buttonCode, mActiveModifiers, btn, btnDesc))
			{
				ButtonData& data = mCachedStates[btn.buttonIdentifier];

				data.button = btn;
				data.state = ButtonState::ToggledOff;
				data.timestamp = event.timestamp;
				data.allowRepeat = btnDesc.repeatable;

				VirtualButtonEvent virtualEvent;
				virtualEvent.button = btn;
				virtualEvent.state = ButtonState::Off;

				mEvents.push(virtualEvent);
			}
		}
	}
}