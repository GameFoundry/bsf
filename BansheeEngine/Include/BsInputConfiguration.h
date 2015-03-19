#pragma once

#include "BsPrerequisites.h"
#include "BsInputFwd.h"

namespace BansheeEngine
{
	/**
	 * @brief	Describes a virtual button. Virtual buttons allow you to
	 *			map custom actions without needing to know about what 
	 *			physical buttons trigger those actions.
	 */
	struct BS_EXPORT VIRTUAL_BUTTON_DESC
	{
		VIRTUAL_BUTTON_DESC();

		/**
		 * @brief	Constructs a virtual button descriptor.
		 *
		 * @param	buttonCode	Physical button the virtual button is triggered by.
		 * @param	modifiers	Modifiers required to be pressed with the physical button to trigger the virtual button.
		 * @param	repeatable	If true, the virtual button events will be sent continually while the physical button is being held.
		 */
		VIRTUAL_BUTTON_DESC(ButtonCode buttonCode, ButtonModifier modifiers = ButtonModifier::None, bool repeatable = false);

		ButtonCode buttonCode;
		ButtonModifier modifiers;
		bool repeatable;
	};

	/**
	 * @brief	Describes a virtual axis. Virtual axes allow you to map
	 *			custom axes without needing to know the actual physical device
	 *			handling those axes.
	 */
	struct BS_EXPORT VIRTUAL_AXIS_DESC
	{
		VIRTUAL_AXIS_DESC();

		/**
		 * @brief	Constructs a new virtual axis descriptor.
		 *
		 * @param	type		Type of physical axis to map to. See InputAxis type for common types, but you are not limited to those values.
		 * @param	deadZone	Value below which to ignore axis value and consider it 0.
		 * @param	sensitivity	Higher sensitivity means the axis will more easily reach its maximum values.
		 * @param	invert		Should axis values be inverted.
		 */
		VIRTUAL_AXIS_DESC(UINT32 type, float deadZone = 0.0001f, float sensitivity = 1.0f, bool invert = false);

		float deadZone;
		float sensitivity;
		bool invert;
		UINT32 type;
	};

	/**
	 * @brief	Identifier for a virtual button. 
	 * 			
	 * @note	Primary purpose of this class is to avoid expensive string compare (i.e. button names),
	 * 			and instead use a unique button identifier for compare. Generally you want to create 
	 * 			one of these using the button name, and then store it for later use. 
	 * 			
	 *			This class is not thread safe and should only be used on the sim thread.
	 *
	 * @see		VIRTUAL_BUTTON_DESC
	 */
	class BS_EXPORT VirtualButton 
	{
	public:
		VirtualButton();
		VirtualButton(const String& name);

		bool operator== (const VirtualButton& rhs) const
		{
			return (buttonIdentifier == rhs.buttonIdentifier);
		}

		UINT32 buttonIdentifier;
	private:
		static Map<String, UINT32> UniqueButtonIds;
		static UINT32 NextButtonId;
	};

	/**
	 * @brief	Identifier for a virtual axis. 
	 * 			
	 * @note	Primary purpose of this class is to avoid expensive string compare (i.e. axis names),
	 * 			and instead use a unique axis identifier for compare. Generally you want to create 
	 * 			one of these using the axis name, and then store it for later use. 
	 * 			
	 *			This class is not thread safe and should only be used on the sim thread.
	 *
	 * @see		VIRTUAL_AXIS_DESC
	 */
	class BS_EXPORT VirtualAxis
	{
	public:
		VirtualAxis();
		VirtualAxis(const String& name);

		UINT32 axisIdentifier;

		bool operator== (const VirtualAxis& rhs) const
		{
			return (axisIdentifier == rhs.axisIdentifier);
		}

	private:
		static Map<String, UINT32> UniqueAxisIds;
		static UINT32 NextAxisId;
	};

	/**
	 * @brief	Contains virtual <-> physical key mappings.
	 */
	class BS_EXPORT InputConfiguration
	{
		static const int MAX_NUM_DEVICES_PER_TYPE = 8;
		static const int MAX_NUM_DEVICES = (UINT32)InputDevice::Count * MAX_NUM_DEVICES_PER_TYPE;

		/**
		 * @brief	Internal virtual button data container.
		 */
		struct VirtualButtonData
		{
			String name;
			VirtualButton button;
			VIRTUAL_BUTTON_DESC desc;
		};

		/**
		 * @brief	Internal virtual axis data container.
		 */
		struct VirtualAxisData
		{
			String name;
			VirtualAxis axis;
			VIRTUAL_AXIS_DESC desc;
		};

		/**
		 * @brief	Internal container for holding axis data for all devices.
		 */
		struct DeviceAxisData
		{
			VirtualAxisData axes[(UINT32)InputAxis::Count];
		};

	public:
		InputConfiguration();

		/**
		 * @brief	Registers a new virtual button.
		 *
		 * @param	name		Unique name of the virtual button. You will use this to access the button.
		 * @param	buttonCode	Physical button the virtual button is triggered by.
		 * @param	modifiers	Modifiers required to be pressed with the physical button to trigger the virtual button.
		 * @param	repeatable	If true, the virtual button events will be sent continually while the physical button is being held.
		 */
		void registerButton(const String& name, ButtonCode buttonCode, ButtonModifier modifiers = ButtonModifier::None, bool repeatable = false);

		/**
		 * @brief	Unregisters a virtual button with the specified name. Events will no longer be generated for that button.
		 */
		void unregisterButton(const String& name);

		/**
		 * @brief	Registers a new virtual axis.
		 *
		 * @param	name	Unique name of the virtual axis. You will use this to access the axis.
		 * @param	desc	Descriptor structure containing virtual axis creation parameters.
		 */
		void registerAxis(const String& name, const VIRTUAL_AXIS_DESC& desc);

		/**
		 * @brief	Unregisters a virtual axis with the specified name. You will no longer
		 *			be able to retrieve valid values for that axis.
		 */
		void unregisterAxis(const String& name);

		/**
		 * @brief	Sets repeat interval for held virtual buttons.
		 */
		void setRepeatInterval(UINT64 milliseconds) { mRepeatInterval = milliseconds; }

		/**
		 * @brief	Gets the currently set repeat interval for held virtual buttons.
		 */
		UINT64 getRepeatInterval() const { return mRepeatInterval; }

		/**
		 * @brief	Returns data about a virtual button from a physical button code and modifier flags.
		 */
		bool _getButton(ButtonCode code, UINT32 modifiers, VirtualButton& btn, VIRTUAL_BUTTON_DESC& btnDesc) const;

		/**
		 * @brief	Retrieves virtual axis descriptor for the provided axis.
		 */
		bool _getAxis(const VirtualAxis& axis, VIRTUAL_AXIS_DESC& axisDesc) const;
	private:
		Vector<VirtualButtonData> mButtons[BC_Count];
		Vector<VirtualAxisData> mAxes;

		UINT64 mRepeatInterval;
	};
}