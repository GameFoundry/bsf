//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsNullPhysicsPrerequisites.h"
#include "Physics/BsCharacterController.h"

namespace bs
{
	/** @addtogroup NullPhysics
	 *  @{
	 */

	/** Null implementation if a CharacterController. */
	class NullPhysicsCharacterController : public CharacterController
	{
	public:
		NullPhysicsCharacterController(const CHAR_CONTROLLER_DESC& desc);
		~NullPhysicsCharacterController() = default;

		/** @copydoc CharacterController::move */
		CharacterCollisionFlags move(const Vector3& displacement) override;

		/** @copydoc CharacterController::getPosition */
		Vector3 getPosition() const override { return mDesc.position; }

		/** @copydoc CharacterController::setPosition */
		void setPosition(const Vector3& position) override { mDesc.position = position; }

		/** @copydoc CharacterController::getFootPosition */
		Vector3 getFootPosition() const override { return mDesc.position; }

		/** @copydoc CharacterController::setFootPosition */
		void setFootPosition(const Vector3& position) override { mDesc.position = position; }

		/** @copydoc CharacterController::getRadius */
		float getRadius() const override { return mDesc.radius; }

		/** @copydoc CharacterController::setRadius */
		void setRadius(float radius) override { mDesc.radius = radius; }

		/** @copydoc CharacterController::getHeight */
		float getHeight() const override { return mDesc.height; }

		/** @copydoc CharacterController::setHeight */
		void setHeight(float height) override { mDesc.height = height; }

		/** @copydoc CharacterController::getUp */
		Vector3 getUp() const override { return mDesc.up; }

		/** @copydoc CharacterController::setUp */
		void setUp(const Vector3& up) override { mDesc.up = up; }

		/** @copydoc CharacterController::getClimbingMode */
		CharacterClimbingMode getClimbingMode() const override { return mDesc.climbingMode; }

		/** @copydoc CharacterController::setClimbingMode */
		void setClimbingMode(CharacterClimbingMode mode) override { mDesc.climbingMode = mode; }

		/** @copydoc CharacterController::getNonWalkableMode */
		CharacterNonWalkableMode getNonWalkableMode() const override { return mDesc.nonWalkableMode; }

		/** @copydoc CharacterController::setNonWalkableMode */
		void setNonWalkableMode(CharacterNonWalkableMode mode) override { mDesc.nonWalkableMode = mode; }

		/** @copydoc CharacterController::getMinMoveDistance */
		float getMinMoveDistance() const override { return mDesc.minMoveDistance; }

		/** @copydoc CharacterController::setMinMoveDistance */
		void setMinMoveDistance(float value) override { mDesc.minMoveDistance = value; }

		/** @copydoc CharacterController::getContactOffset */
		float getContactOffset() const override { return mDesc.contactOffset; }

		/** @copydoc CharacterController::setContactOffset */
		void setContactOffset(float value) override { mDesc.contactOffset = value; }

		/** @copydoc CharacterController::getStepOffset */
		float getStepOffset() const override { return mDesc.stepOffset; }

		/** @copydoc CharacterController::setStepOffset */
		void setStepOffset(float value) override { mDesc.stepOffset = value; }

		/** @copydoc CharacterController::getSlopeLimit */
		Radian getSlopeLimit() const override { return mDesc.slopeLimit; }

		/** @copydoc CharacterController::setSlopeLimit */
		void setSlopeLimit(Radian value) override { mDesc.slopeLimit = value; }

	private:
		CHAR_CONTROLLER_DESC mDesc;
	};

	/** @} */
}
