#pragma once

#include "BsPrerequisites.h"
#include "BsModule.h"
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

		struct DeviceData
		{
			Map<UINT32, ButtonData> cachedStates;
		};

		struct VirtualButtonEvent
		{
			VirtualButton button;
			ButtonState state;
			UINT32 deviceIdx;
		};

	public:
		VirtualInput();

		static std::shared_ptr<InputConfiguration> createConfiguration();

		void setConfiguration(const std::shared_ptr<InputConfiguration>& input);
		std::shared_ptr<InputConfiguration> getConfiguration() const { return mInputConfiguration; }

		bool isButtonDown(const VirtualButton& button, UINT32 deviceIdx = 0) const;
		bool isButtonUp(const VirtualButton& button, UINT32 deviceIdx = 0) const;
		bool isButtonHeld(const VirtualButton& button, UINT32 deviceIdx = 0) const;

		float getAxisValue(const VirtualAxis& axis, UINT32 deviceIdx = 0) const;

		void update();

		Event<void(const VirtualButton&, UINT32 deviceIdx)> onButtonDown;
		Event<void(const VirtualButton&, UINT32 deviceIdx)> onButtonUp;
		Event<void(const VirtualButton&, UINT32 deviceIdx)> onButtonHeld;
	private:
		friend class VirtualButton;

		void buttonDown(const ButtonEvent& event);
		void buttonUp(const ButtonEvent& event);

		std::shared_ptr<InputConfiguration> mInputConfiguration;
		Vector<DeviceData> mDevices;
		Queue<VirtualButtonEvent> mEvents;
		UINT32 mActiveModifiers;
	};
}