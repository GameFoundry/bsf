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

		virtual bool intersects(const Ray& ray, float& t) const = 0;
		virtual void update(const HCamera& camera, const Vector2I& pointerPos, const Ray& ray) = 0;

		State getState() const { return mState; }
		bool getFixedScale() const { return mFixedScale; }
		float getSnapValue() const { return mSnapValue; }

		void setPosition(const Vector3& position);
		void setRotation(const Quaternion& rotation);
		void setScale(const Vector3& scale);

		const Vector3& getPosition() const { return mPosition; }
		const Quaternion& getRotation() const { return mRotation; }
		const Vector3& getScale() const { return mScale; }

	protected:
		friend class HandleSliderManager;

		void setInactive() { mState = State::Inactive; reset(); }
		void setActive(const Vector2I& pointerPos) { mState = State::Active; mLastPointerPos = pointerPos; }
		void setHover() { mState = State::Hover; reset(); }

		const Matrix4& getTransform() const;
		const Matrix4& getTransformInv() const;

		virtual void updateCachedTransform() const;
		virtual void reset() = 0;

		float calcDelta(const HCamera& camera, const Vector3& position, const Vector3& direction, 
			const Vector2I& pointerStart, const Vector2I& pointerEnd);

		bool mFixedScale;
		float mSnapValue;

		Vector3 mPosition;
		Quaternion mRotation;
		Vector3 mScale;

		Vector2I mLastPointerPos;
		Vector2I mCurPointerPos;
		State mState;

		mutable bool mTransformDirty;
		mutable Matrix4 mTransform;
		mutable Matrix4 mTransformInv;
	};
}