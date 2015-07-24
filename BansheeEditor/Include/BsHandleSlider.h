#pragma once

#include "BsEditorPrerequisites.h"
#include "BsVector2I.h"
#include "BsMatrix4.h"
#include "BsQuaternion.h"

namespace BansheeEngine
{
	/**
	 * @brief	Base class for all handle sliders. A handle slider is geometry that the user
	 *			can interact with by selecting or dragging (i.e. sliding) it. Sliders generally
	 *			output a one- or multi-dimensional delta value that signals the drag amount 
	 *			(and/or direction).
	 */
	class BS_ED_EXPORT HandleSlider
	{
	public:
		/**
		 * @brief	Possible states the slider can be in.
		 */
		enum class State
		{
			Inactive, /**< Slider is not being interacted with. */
			Active, /**< Slider is clicked on and being dragged. */
			Hover /**< Slider is being hovered over but isn't clicked on. */
		};

		/**
		 * @brief	Constructs a new handle slider.
		 *
		 * @param	fixedScale	If true the handle slider will always try to maintain the same visible
		 *						area in the viewport regardless of distance from camera. 
		 */
		HandleSlider(bool fixedScale);
		virtual ~HandleSlider() { }

		/**
		 * @brief	Attempts to find an intersection between the provided ray and the slider geometry.
		 *
		 * @param	ray	Ray in world space to try to interect with geometry.
		 * @param	t	Position of the intersection along the ray. Only if intersection happened.
		 *
		 * @return	Whether an intersection was detected.
		 */
		virtual bool intersects(const Ray& ray, float& t) const = 0;

		/**
		 * @brief	Updates a slider that is currently active (being dragged).
		 *
		 * @param	camera		Camera through which we're interacting with the slider.
		 * @param	inputDelta	Pointer movement since the last time this method was called.
		 */
		virtual void handleInput(const CameraHandlerPtr& camera, const Vector2I& inputDelta) = 0;

		/**
		 * @brief	Updates the state of the slider. Must be called every frame.
		 *
		 * @param	camera	Camera through which we're interacting with the slider.
		 */
		void update(const CameraHandlerPtr& camera);

		/**
		 * @brief	Returns the state the slider is currently in.
		 */
		State getState() const { return mState; }
		
		/**
		 * @brief	Returns if fixed scale is enabled. If enabled the handle slider will 
		 *			always try to maintain the same visible area in the viewport regardless 
		 *			of distance from camera.
		 */
		bool getFixedScale() const { return mFixedScale; }

		/**
		 * @brief	Sets the world position of the slider.
		 */
		void setPosition(const Vector3& position);

		/**
		 * @brief	Sets the world rotation of the slider.
		 */
		void setRotation(const Quaternion& rotation);

		/**
		 * @brief	Sets the scale of the slider.
		 */
		void setScale(const Vector3& scale);


		/**
		 * @brief	Gets the world position of the slider.
		 */
		const Vector3& getPosition() const { return mPosition; }

		/**
		 * @brief	Gets the world rotation of the slider.
		 */
		const Quaternion& getRotation() const { return mRotation; }

		/**
		 * @brief	Gets the scale of the slider.
		 */
		const Vector3& getScale() const { return mScale; }

	protected:
		friend class HandleSliderManager;

		/**
		 * @brief	Toggles the slider state to inactive.
		 */
		void setInactive();

		/**
		 * @brief	Toggles the slider state to active.
		 *
		 * @param	camera		Camera through which the slider was activated.
		 * @param	pointerPos	Position of the pointer when the slider was activated.
		 */
		void setActive(const CameraHandlerPtr& camera, const Vector2I& pointerPos);

		/**
		 * @brief	Toggles the slider state to hovered.
		 */
		void setHover();

		/**
		 * @brief	Gets the slider transform depending on set position, rotation and scale values.
		 */
		const Matrix4& getTransform() const;

		/**
		 * @brief	Gets the inverse of the slider transform depending on 
		 *			set position, rotation and scale values.
		 */
		const Matrix4& getTransformInv() const;

		/**
		 * @brief	Triggered when the slider state is changed to active.
		 */
		virtual void activate(const CameraHandlerPtr& camera, const Vector2I& pointerPos) { }

		/**
		 * @brief	Triggered when the slider state is changed from active to some other state.
		 */
		virtual void reset() { }

		/**
		 * @brief	Updates the internal transform from the stored position, rotation and scale values.
		 */
		void updateCachedTransform() const;

		/**
		 * @brief	Calculates amount of movement along the provided ray depending on pointer movement.
		 *
		 * @param	camera			Camera on which the pointer movement is occurring.
		 * @param	position		Position of the ray to calculate movement on.
		 * @param	direction		Direction of the ray to calculate movement on. Must be normalized.
		 * @param	pointerStart	Starting position of the pointer when movement started, in pixels relative to provided camera.
		 * @param	pointerEnd		Current position of the pointer, in pixels relative to provided camera.
		 */
		float calcDelta(const CameraHandlerPtr& camera, const Vector3& position, const Vector3& direction,
			const Vector2I& pointerStart, const Vector2I& pointerEnd);

		bool mFixedScale;

		Vector3 mPosition;
		Quaternion mRotation;
		Vector3 mScale;
		float mDistanceScale;

		Vector2I mStartPointerPos;
		Vector2I mCurrentPointerPos;
		State mState;

		mutable bool mTransformDirty;
		mutable Matrix4 mTransform;
		mutable Matrix4 mTransformInv;
	};
}