#pragma once

#include "BsPrerequisites.h"
#include "CmInputFwd.h"

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
		VIRTUAL_BUTTON_DESC(CM::ButtonCode buttonCode, VButtonModifier modifiers = VButtonModifier::None, bool repeatable = false);

		CM::ButtonCode buttonCode;
		VButtonModifier modifiers;
		bool repeatable;
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
		VirtualButton(const CM::String& name);

		CM::UINT32 buttonIdentifier;

		bool operator== (const VirtualButton& rhs) const
		{
			return (buttonIdentifier == rhs.buttonIdentifier);
		}

	private:
		static CM::Map<CM::String, CM::UINT32>::type UniqueButtonIds;
		static CM::UINT32 NextButtonId;
	};

	class BS_EXPORT InputConfiguration
	{
		struct VirtualButtonData
		{
			CM::String name;
			VirtualButton button;
			VIRTUAL_BUTTON_DESC desc;
		};

	public:
		InputConfiguration();

		void registerButton(const CM::String& name, CM::ButtonCode buttonCode, VButtonModifier modifiers = VButtonModifier::None, bool repeatable = false);
		void unregisterButton(const CM::String& name);

		void setRepeatInterval(CM::UINT64 milliseconds) { mRepeatInterval = milliseconds; }
		CM::UINT64 getRepeatInterval() const { return mRepeatInterval; }

		bool getButton(CM::ButtonCode code, CM::UINT32 modifiers, VirtualButton& btn, VIRTUAL_BUTTON_DESC& btnDesc) const;

		// TODO - registerAxis
	private:
		CM::Vector<VirtualButtonData>::type mButtons[CM::BC_Count];

		CM::UINT64 mRepeatInterval;
	};
}