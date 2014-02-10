#pragma once

#include "BsPrerequisites.h"
#include "CmModule.h"
#include "BsInputConfiguration.h"

namespace BansheeEngine
{
	class BS_EXPORT VirtualInput : public CM::Module<VirtualInput>
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
			CM::UINT64 timestamp;
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
		const std::shared_ptr<InputConfiguration>& getConfiguration() const { return mInputConfiguration; }

		bool isButtonDown(const VirtualButton& button) const;
		bool isButtonUp(const VirtualButton& button) const;
		bool isButtonHeld(const VirtualButton& button) const;

		void update();

		boost::signal<void(const VirtualButton&)> onButtonDown;
		boost::signal<void(const VirtualButton&)> onButtonUp;
		boost::signal<void(const VirtualButton&)> onButtonHeld;
	private:
		friend class VirtualButton;

		std::shared_ptr<InputConfiguration> mInputConfiguration;
		CM::Map<CM::UINT32, ButtonData>::type mCachedStates;
		CM::Queue<VirtualButtonEvent>::type mEvents;
		CM::UINT32 mActiveModifiers;

		void buttonDown(const CM::ButtonEvent& event);
		void buttonUp(const CM::ButtonEvent& event);
	};
}