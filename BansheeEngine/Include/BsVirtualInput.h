#pragma once

#include "BsPrerequisites.h"
#include "CmModule.h"
#include "BsInputConfiguration.h"

namespace BansheeEngine
{
	class BS_EXPORT VirtualInput : public Module<VirtualInput>
	{
		enum class ButtonState
		{
			Off,
			On,
			ToggledOn,
			ToggledOff
		};

		struct ButtonData
		{
			VirtualButton button;
			ButtonState state;
			UINT64 timestamp;
			bool allowRepeat;
		};

		struct VirtualButtonEvent
		{
			VirtualButton button;
			ButtonState state;
		};

	public:
		VirtualInput();

		static std::shared_ptr<InputConfiguration> createConfiguration();

		void setConfiguration(const std::shared_ptr<InputConfiguration>& input);
		std::shared_ptr<InputConfiguration> getConfiguration() const { return mInputConfiguration; }

		bool isButtonDown(const VirtualButton& button) const;
		bool isButtonUp(const VirtualButton& button) const;
		bool isButtonHeld(const VirtualButton& button) const;

		void update();

		Event<void(const VirtualButton&)> onButtonDown;
		Event<void(const VirtualButton&)> onButtonUp;
		Event<void(const VirtualButton&)> onButtonHeld;
	private:
		friend class VirtualButton;

		std::shared_ptr<InputConfiguration> mInputConfiguration;
		Map<UINT32, ButtonData> mCachedStates;
		Queue<VirtualButtonEvent> mEvents;
		UINT32 mActiveModifiers;

		void buttonDown(const ButtonEvent& event);
		void buttonUp(const ButtonEvent& event);
	};
}