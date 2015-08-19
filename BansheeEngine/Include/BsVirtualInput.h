#pragma once

#include "BsPrerequisites.h"
#include "BsModule.h"
#include "BsInputConfiguration.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles virtual input that allows you to receive virtual input events that 
	 *			hide the actual physical input, allowing you to easily change the input keys
	 *			while being transparent to the external code.
	 */
	class BS_EXPORT VirtualInput : public Module<VirtualInput>
	{
		/**
		 * @brief	Possible states of virtual buttons.
		 */
		enum class ButtonState
		{
			Off,
			On,
			ToggledOn,
			ToggledOff
		};

		/**
		 * @brief	Data container for a single virtual button.
		 */
		struct ButtonData
		{
			VirtualButton button;
			ButtonState state;
			UINT64 timestamp;
			UINT64 updateFrameIdx;
			bool allowRepeat;
		};

		/**
		 * @brief	Contains button data for a specific input device.
		 */
		struct DeviceData
		{
			Map<UINT32, ButtonData> cachedStates;
		};

		/**
		 * @brief	Data container for a virtual button event.
		 */
		struct VirtualButtonEvent
		{
			VirtualButton button;
			ButtonState state;
			UINT32 deviceIdx;
		};

	public:
		VirtualInput();

		/**
		 * @brief	Creates a new empty input configuration.
		 */
		static std::shared_ptr<InputConfiguration> createConfiguration();

		/**
		 * @brief	Sets an input configuration that determines how physical keys map to virtual buttons.
		 */
		void setConfiguration(const std::shared_ptr<InputConfiguration>& input);

		/**
		 * @brief	Retrieves the active input configuration that determines how physical keys map to virtual buttons.
		 */
		std::shared_ptr<InputConfiguration> getConfiguration() const { return mInputConfiguration; }

		/**
		 * @brief	Check is the virtual button just getting pressed. This state is only
		 *			active for one frame.
		 *
		 * @param	button		Virtual button identifier.
		 * @param	deviceIdx	Optional device index in case multiple input devices are available.
		 */
		bool isButtonDown(const VirtualButton& button, UINT32 deviceIdx = 0) const;

		/**
		 * @brief	Check is the virtual button just getting released. This state is only
		 *			active for one frame.
		 *
		 * @param	button		Virtual button identifier.
		 * @param	deviceIdx	Optional device index in case multiple input devices are available.
		 */
		bool isButtonUp(const VirtualButton& button, UINT32 deviceIdx = 0) const;

		/**
		 * @brief	Check is the virtual button is being held. This state is active as long
		 *			as the button is being held down, i.e. possibly multiple frames.
		 *
		 * @param	button		Virtual button identifier.
		 * @param	deviceIdx	Optional device index in case multiple input devices are available.
		 */
		bool isButtonHeld(const VirtualButton& button, UINT32 deviceIdx = 0) const;

		/**
		 * @brief	Returns normalized value for the specified input axis. 
		 *			Returned value will usually be in [-1.0, 1.0] range, but can be outside
		 *			the range for devices with unbound axes (e.g. mouse).
		 *
		 * @param	axis		Virtual axis identifier.
		 * @param	deviceIdx	Optional device index in case multiple input devices are available.
		 */
		float getAxisValue(const VirtualAxis& axis, UINT32 deviceIdx = 0) const;

		/**
		 * @brief	Called once every frame.
		 *
		 * @note	Internal method.
		 */
		void _update();

		/**
		 * @brief	Triggered when a virtual button is pressed.
		 */
		Event<void(const VirtualButton&, UINT32 deviceIdx)> onButtonDown;

		/**
		 * @brief	Triggered when a virtual button is released.
		 */
		Event<void(const VirtualButton&, UINT32 deviceIdx)> onButtonUp;

		/**
		 * @brief	Triggered every frame when a virtual button is being held down.
		 */
		Event<void(const VirtualButton&, UINT32 deviceIdx)> onButtonHeld;
	private:
		friend class VirtualButton;

		/**
		 * @brief  Performs all logic related to a button press.
		 */
		void buttonDown(const ButtonEvent& event);

		/**
		 * @brief  Performs all logic related to a button release.
		 */
		void buttonUp(const ButtonEvent& event);

		std::shared_ptr<InputConfiguration> mInputConfiguration;
		Vector<DeviceData> mDevices;
		Queue<VirtualButtonEvent> mEvents;
		UINT32 mActiveModifiers;

		// Transient
		Vector<VirtualButton> tempButtons;
		Vector<VIRTUAL_BUTTON_DESC> tempBtnDescs;
	};

	/**
	 * @copydoc	VirtualInput
	 */
	BS_EXPORT VirtualInput& gVirtualInput();
}