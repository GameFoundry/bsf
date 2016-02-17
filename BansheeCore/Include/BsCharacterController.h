//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsVector3.h"

namespace BansheeEngine
{
	/** 
	 * Controls climbing behaviour for a capsule character controller. Normally the character controller will not
	 * automatically climb when heights are greater than the assigned step offset. However due to the shape of the capsule
	 * it might automatically climb over slightly larger heights than assigned step offsets.
	 */
	enum class CharacterClimbingMode
	{
		Normal, /**< Normal behaviour. Capsule character controller will be able to auto-step over the step offset. */
		Constrained /**< The system will attempt to limit auto-step to the provided step offset and no higher. */
	};

	/** Controls behaviour when a character controller reaches a slope thats larger than its slope offset. */
	enum class CharacterNonWalkableMode
	{
		Prevent, /**< Character will be prevented from going further, but will be allowed to move laterally. */
		PreventAndSlide /**< Character will be prevented from going further, but also slide down the slope. */
	};

	/** Reports in which directions is the character colliding with other objects */
	enum class CharacterCollisionFlag
	{
		Sides = 0x1, /**< Character is colliding with its sides. */
		Up = 0x2, /**< Character is colliding with the ceiling. */
		Down = 0x4 /**< Character is colliding with the ground. */
	};

	/** @copydoc CharacterCollisionFlag */
	typedef Flags<CharacterCollisionFlag> CharacterCollisionFlags;
	BS_FLAGS_OPERATORS(CharacterCollisionFlag)

	class BS_CORE_EXPORT CharacterController
	{
	public:
		virtual ~CharacterController() { }

		/**
		 * Moves the controller in the specified direction by the specified amount, while interacting with surrounding
		 * geometry. Returns flags signaling where collision occurred after the movement.
		 */
		virtual CharacterCollisionFlags move(const Vector3& displacement) = 0;

		/** Returns position of the center of the controller. */
		virtual Vector3 getPosition() const = 0;

		/** 
		 * Sets position of the center of the controller. This will teleport the character to the location. Use move()
		 * for movement that includes physics.
		 */
		virtual void setPosition(const Vector3& position) = 0;

		/** Returns position of the bottom of the controller. Position takes contact offset into account. */
		virtual Vector3 getFootPosition() const = 0;

		/** 
		 * Sets position of the bottom of the controller. Position takes contact offset into account. This will teleport the
		 * character to the location. Use move() for movement that includes physics.
		 */
		virtual void setFootPosition(const Vector3& position) = 0;

		/** Returns the radius of the controller capsule. */
		virtual float getRadius() const = 0;

		/** Sets the radius of the controller capsule. */
		virtual void setRadius(float radius) = 0;

		/** Returns the height of the controller capsule. */
		virtual float getHeight() const = 0;

		/** Sets the height of the controller capsule. */
		virtual void setHeight(float height) = 0;

		/** Returns the up direction of capsule. Determines capsule orientation. */
		virtual Vector3 getUp() const = 0;

		/** Sets the up direction of capsule. Determines capsule orientation. */
		virtual void setUp(const Vector3& up) = 0;

		/** 
		 * Returns climbing mode that controls what happens when character encounters a height higher than its step offset. 
		 *
		 * @see	CharacterClimbingMode
		 */
		virtual CharacterClimbingMode getClimbingMode() const = 0;

		/** 
		 * Sets climbing mode that controls what happens when character encounters a height higher than its step offset. 
		 *
		 * @see	CharacterClimbingMode
		 */
		virtual void setClimbingMode(CharacterClimbingMode mode) = 0;

		/** 
		 * Returns non walkable mode that controls what happens when character encounters a slope higher than its slope 
		 * offset. 
		 *
		 * @see	CharacterNonWalkableMode
		 */
		virtual CharacterNonWalkableMode getNonWalkableMode() const = 0;

		/** 
		 * Sets non walkable mode that controls what happens when character encounters a slope higher than its slope 
		 * offset. 
		 *
		 * @see	CharacterNonWalkableMode
		 */
		virtual void setNonWalkableMode(CharacterNonWalkableMode mode) = 0;

		/** 
		 * Returns minimum distance that the character will move during a call to move(). This is used to stop the recursive
		 * motion algorithm when the remaining distance is too small.
		 */
		virtual float getMinMoveDistance() = 0;

		/** 
		 * Sets minimum distance that the character will move during a call to move(). This is used to stop the recursive
		 * motion algorithm when the remaining distance is too small.
		 */
		virtual void setMinMoveDistance(float value) = 0;

		/** 
		 * Returns the contact offset value. Contact offset specifies a skin around the object within which contacts will
		 * be generated. It should be a small positive non-zero value.
		 */
		virtual float getContactOffset() = 0;

		/**
		 * Sets the contact offset value. Contact offset specifies a skin around the object within which contacts will
		 * be generated. It should be a small positive non-zero value.
		 */
		virtual void setContactOffset(float value) = 0;

		/** 
		 * Gets the step offset that controls which obstacles will the character be able to automatically step over without
		 * being stopped. This is the height of the maximum obstacle that will be stepped over (with exceptions, see
		 * setClimbingMode().
		 */
		virtual float getStepOffset() = 0;

		/** 
		 * Sets the step offset that controls which obstacles will the character be able to automatically step over without
		 * being stopped. This is the height of the maximum obstacle that will be stepped over (with exceptions, see
		 * setClimbingMode().
		 */
		virtual void setStepOffset(float value) = 0;

		/**
		 * Gets the slope angle that controls which slopes should the character consider too steep and won't be able to
		 * move over. See setNonWalkableMode() for more information.
		 */
		virtual Radian getSlopeOffset() = 0;

		/**
		 * Sets the slope angle that controls which slopes should the character consider too steep and won't be able to
		 * move over. See setNonWalkableMode() for more information.
		 */
		virtual void setSlopeOffset(Radian value) = 0;

		/** Creates a new character controller. */
		static SPtr<CharacterController> create();
	};
}