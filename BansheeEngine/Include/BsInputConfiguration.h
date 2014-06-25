#pragma once

#include "BsPrerequisites.h"
#include "BsInputFwd.h"

namespace BansheeEngine
{
	enum class VButtonModifier
	{
		None = 0x00,
		Shift = 0x01,
		Ctrl = 0x02,
		Alt = 0x04,
		ShiftCtrl = 0x03,
		CtrlAlt = 0x06,
		ShiftAlt = 0x05,
		ShiftCtrlAlt = 0x07
	};

	struct VIRTUAL_BUTTON_DESC
	{
		VIRTUAL_BUTTON_DESC();
		VIRTUAL_BUTTON_DESC(ButtonCode buttonCode, VButtonModifier modifiers = VButtonModifier::None, bool repeatable = false);

		ButtonCode buttonCode;
		VButtonModifier modifiers;
		bool repeatable;
	};

	struct VIRTUAL_AXIS_DESC
	{
		VIRTUAL_AXIS_DESC();
		VIRTUAL_AXIS_DESC(InputAxis type, float deadZone = 0.0001f, float sensitivity = 1.0f, bool invert = false, bool smooth = true);

		float deadZone;
		float sensitivity;
		bool invert;
		bool smooth;
		InputAxis type;
	};

	/**
	 * @brief	Identifier for a virtual button. 
	 * 			
	 * @note	Primary purpose of this class is to avoid expensive string compare (i.e. button names),
	 * 			and instead use a unique button identifier for compare. Generally you want to create 
	 * 			one of these using the button name, and then store it for later use. 
	 * 			
	 *			This class is not thread safe and should only be used on the sim thread.
	 */
	class BS_EXPORT VirtualButton 
	{
	public:
		VirtualButton();
		VirtualButton(const String& name);

		UINT32 buttonIdentifier;

		bool operator== (const VirtualButton& rhs) const
		{
			return (buttonIdentifier == rhs.buttonIdentifier);
		}

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

	class BS_EXPORT InputConfiguration
	{
		static const int MAX_NUM_DEVICES_PER_TYPE = 8;
		static const int MAX_NUM_DEVICES = (UINT32)InputDevice::Count * MAX_NUM_DEVICES_PER_TYPE;

		struct VirtualButtonData
		{
			String name;
			VirtualButton button;
			VIRTUAL_BUTTON_DESC desc;
		};

		struct VirtualAxisData
		{
			String name;
			VirtualAxis axis;
			VIRTUAL_AXIS_DESC desc;
		};

		struct DeviceAxisData
		{
			VirtualAxisData axes[(UINT32)InputAxis::Count];
		};

	public:
		InputConfiguration();

		void registerButton(const String& name, ButtonCode buttonCode, VButtonModifier modifiers = VButtonModifier::None, bool repeatable = false);
		void unregisterButton(const String& name);

		void registerAxis(const String& name, const VIRTUAL_AXIS_DESC& desc);
		void unregisterAxis(const String& name);

		void setRepeatInterval(UINT64 milliseconds) { mRepeatInterval = milliseconds; }
		UINT64 getRepeatInterval() const { return mRepeatInterval; }

		bool _getButton(ButtonCode code, UINT32 modifiers, VirtualButton& btn, VIRTUAL_BUTTON_DESC& btnDesc) const;
		bool _getAxis(const VirtualAxis& axis, VIRTUAL_AXIS_DESC& axisDesc) const;
	private:
		Vector<VirtualButtonData> mButtons[BC_Count];
		Vector<VirtualAxisData> mAxes;

		UINT64 mRepeatInterval;
	};
}