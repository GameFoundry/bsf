#pragma once

#include "CmPrerequisites.h"
#include "CmInputFwd.h"
#include <boost/signal.hpp>
#include "CmVector2I.h"

namespace BansheeEngine
{
	struct RawAxisState
	{
		Vector2I rel;
		Vector2I abs;
	};

	enum class RawInputAxis
	{
		Mouse_XY,
		Mouse_Z,
		Joy_1,
		Joy_2,
		Joy_3,
		Joy_4,
		Joy_5,
		Joy_6,
		Joy_7,
		Joy_8,
		Joy_9,
		Joy_10,
		Joy_11,
		Joy_12,
		Joy_13,
		Joy_14,
		Joy_15,
		Joy_16,
		Count
	};

	/**
	 * @brief	Represents a specific way of acquiring low-level input. InputManager (which provides a higher level input)
	 * 			must have at least one InputHandler attached. Attach events handler to the provided signals to handle input.
	 */
	class CM_EXPORT RawInputHandler
	{
	public:
		RawInputHandler() {}
		virtual ~RawInputHandler() {}

		boost::signal<void(ButtonCode, UINT64)> onButtonDown;
		boost::signal<void(ButtonCode, UINT64)> onButtonUp;

		boost::signal<void(const RawAxisState&, RawInputAxis)> onAxisMoved;

		/**
		 * @brief	Called every frame by InputManager. Capture input here if needed.
		 */
		virtual void update() {}

		/**
		 * @brief	Called by InputManager whenever window in focus changes.
		 */
		virtual void inputWindowChanged(const RenderWindow& win) {}
	};
}