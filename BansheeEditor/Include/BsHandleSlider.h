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

		HandleSlider(bool fixedScale);
		virtual ~HandleSlider() { }

		virtual bool intersects(const Ray& ray, float& t) const = 0;
		virtual void handleInput(const CameraHandlerPtr& camera, const Vector2I& inputDelta) = 0;
		void update(const CameraHandlerPtr& camera);

		State getState() const { return mState; }
		bool getFixedScale() const { return mFixedScale; }

		void setPosition(const Vector3& position);
		void setRotation(const Quaternion& rotation);
		void setScale(const Vector3& scale);

		const Vector3& getPosition() const { return mPosition; }
		const Quaternion& getRotation() const { return mRotation; }
		const Vector3& getScale() const { return mScale; }

	protected:
		friend class HandleSliderManager;

		void setInactive();
		void setActive(const CameraHandlerPtr& camera, const Vector2I& pointerPos);
		void setHover();

		const Matrix4& getTransform() const;
		const Matrix4& getTransformInv() const;

		virtual void activate(const CameraHandlerPtr& camera, const Vector2I& pointerPos) { }
		virtual void reset() { }
		virtual void updateCachedTransform() const;

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