#pragma once

#include "BsEditorPrerequisites.h"
#include "BsVector2I.h"
#include "BsMatrix4.h"
#include "BsQuaternion.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT HandleSlider
	{
	public:
		enum class State
		{
			Inactive,
			Active,
			Hover
		};

		HandleSlider(bool fixedScale, float snapValue);
		virtual ~HandleSlider() { }

		State getState() const { return mState; }
		bool getFixedScale() const { return mFixedScale; }
		float getSnapValue() const { return mSnapValue; }

		void setPosition(const Vector3& position);
		void setRotation(const Quaternion& rotation);
		void setScale(const Vector3& scale);

		const Vector3& getPosition() const { return mPosition; }
		const Quaternion& getRotation() const { return mRotation; }
		const Vector3& getScale() const { return mScale; }

		virtual const Matrix4& getTransform() const { return mTransform; }

	protected:
		friend class HandleManager;

		void setInactive() { mState = State::Inactive; }
		void setActive(const Vector2I& pointerPos) { mState = State::Active; mLastPointerPos = pointerPos; }
		void setHover() { mState = State::Hover; }
		void registerDrag(const Vector2I& pointerPos);

		bool mFixedScale;
		float mSnapValue;

		Vector3 mPosition;
		Quaternion mRotation;
		Vector3 mScale;

		Matrix4 mTransform;

		Vector2I mLastPointerPos;
		Vector2I mCurPointerPos;
		State mState;
	};
}