#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"
#include "BsDegree.h"

namespace BansheeEngine
{
	/**
	 * @brief	The central place for interacting with and drawing handles.
	 */
	class BS_ED_EXPORT HandleManager : public Module<HandleManager>
	{
	public:
		HandleManager();
		virtual ~HandleManager();

		/**
		 * @brief	To be called every frame. Updates interactable handle sliders based on provided input
		 *			and queues handles for drawing.
		 *
		 * @param	camera		Camera that the input positions are relative to, and destination to draw the handles to.
		 * @param	inputPos	Position of the pointer, relative to the provided camera viewport.
		 * @param	inputDelta	Determines pointer movement since last call to this method.
		 */
		void update(const CameraHandlerPtr& camera, const Vector2I& inputPos, const Vector2I& inputDelta);

		/**
		 * @brief	Select a handle slider at the specified location, if there is any under the pointer. Makes
		 *			the selected slider active and draggable.
		 *
		 * @param	camera		Camera that the input positions are relative to, and destination to draw the handles to.
		 * @param	inputPos	Position of the pointer, relative to the provided camera viewport.
		 */
		void trySelect(const CameraHandlerPtr& camera, const Vector2I& inputPos);

		/**
		 * @brief	Clears the currently selected/active handle slider.
		 */
		void clearSelection();

		/**
		 * @brief	Is any handle slider selected/active.
		 */
		bool isHandleActive() const;

		/**
		 * @brief	Returns the manager that can be used for interacting with handle sliders.
		 */
		HandleSliderManager& getSliderManager() const { return *mSliderManager; }

		/**
		 * @brief	Returns the manager that can be used for drawing handle geometry.
		 */
		HandleDrawManager& getDrawManager() const { return *mDrawManager; }

		/**
		 * @brief	Returns the uniform size for a handle rendered in @p camera, at the world
		 *			position @p handlePos. The handles will be scaled so that they appear
		 *			the same size regardless of distance from camera.
		 */
		float getHandleSize(const CameraHandlerPtr& camera, const Vector3& handlePos) const;

		/**
		 * @brief	Sets the default handle size. This controls the uniform scale returned from
		 *			::getHandleSize method.
		 */
		void setDefaultHandleSize(float value) { mDefaultHandleSize = value; }

		/**
		 * @brief	Sets editor settings that will be used for controlling various 
		 *			handle behaviour.
		 */
		void setSettings(const EditorSettingsPtr& settings);

	protected:
		/**
		 * @brief	Updates the internal properties from editor settings.
		 */
		void updateFromEditorSettings();

		/**
		 * @brief	Called during handle update. Allows handle sliders to be created or 
		 *			destroyed before any input is handled.
		 */
		virtual void refreshHandles() = 0;

		/**
		 * @brief	Called during handle update after handle input is processed.
		 *			Allows implementation to respond to delta values calculated in sliders
		 *			due to input.
		 */
		virtual void triggerHandles() = 0;

		/**
		 * @brief	Called during handle update. Allows implementation to
		 *			queue handle draw commands.
		 */
		virtual void queueDrawCommands() = 0;

		HandleSliderManager* mSliderManager;
		HandleDrawManager* mDrawManager;

		float mDefaultHandleSize = 20.0f;

		EditorSettingsPtr mSettings;
		UINT32 mSettingsHash;
	};
}