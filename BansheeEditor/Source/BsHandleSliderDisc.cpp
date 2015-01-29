#include "BsHandleSliderDisc.h"
#include "BsHandleManager.h"
#include "BsHandleSliderManager.h"
#include "BsRay.h"
#include "BsVector3.h"
#include "BsQuaternion.h"
#include "BsCamera.h"

// DEBUG ONLY
#include "BsDebug.h"

namespace BansheeEngine
{
	const float HandleSliderDisc::TORUS_RADIUS = 0.1f;

	HandleSliderDisc::HandleSliderDisc(const Vector3& normal, float radius, bool fixedScale)
		:HandleSlider(fixedScale), mRadius(radius), mNormal(normal), mDelta(0.0f)
	{
		mCollider = Torus(normal, radius, TORUS_RADIUS);

		HandleSliderManager& sliderManager = HandleManager::instance().getSliderManager();
		sliderManager._registerSlider(this);
	}

	HandleSliderDisc::~HandleSliderDisc()
	{
		HandleSliderManager& sliderManager = HandleManager::instance().getSliderManager();
		sliderManager._unregisterSlider(this);
	}

	bool HandleSliderDisc::intersects(const Ray& ray, float& t) const
	{
		Ray localRay = ray;
		localRay.transform(getTransformInv());

		auto intersect = mCollider.intersects(localRay);

		if (intersect.first)
		{
			t = intersect.second;

			return true;
		}

		return false;
	}

	Vector3 HandleSliderDisc::calculateClosestPointOnArc(const Ray& inputRay, const Vector3& center, const Vector3& up,
		float radius, Degree startAngle, Degree angleAmount)
	{
		Vector3 arcBasis[3];
		arcBasis[1] = up;
		arcBasis[1].orthogonalComplement(arcBasis[2], arcBasis[0]);

		Matrix4 worldToPlane = Matrix4::IDENTITY;
		worldToPlane.setColumn(0, (Vector4)arcBasis[0]);
		worldToPlane.setColumn(1, (Vector4)arcBasis[1]);
		worldToPlane.setColumn(2, (Vector4)arcBasis[2]);
		worldToPlane.setColumn(3, (Vector4)worldToPlane.multiplyAffine(-center));
		worldToPlane[3][3] = 1;

		Plane plane(up, (-center).dot(up));
		Vector3 pointOnPlane;

		auto intersectResult = plane.intersects(inputRay);

		float t = 0.0f;
		if (intersectResult.first)
			pointOnPlane = inputRay.getPoint(intersectResult.second);
		else
			pointOnPlane = Vector3::ZERO;

		pointOnPlane = worldToPlane.multiplyAffine(pointOnPlane);
		Vector2 pointOnPlane2D(pointOnPlane.x, pointOnPlane.z); // y always 0

		Vector2 closestPoint2D;
		float dist = pointOnPlane2D.length();
		if (dist > 0.0f)
			closestPoint2D = mRadius * (pointOnPlane2D / dist);
		else
			closestPoint2D = Vector2(mRadius, 0);

		Radian angle = Math::atan2(-closestPoint2D.y, -closestPoint2D.x) + Math::PI;

		float angleRad = angle.valueRadians();
		float angleAmountRad = Math::clamp(angleAmount.valueRadians(), 0.0f, Math::PI * 2);

		float startAngleRad = startAngle.wrap().valueRadians();
		float endAngleRad = startAngleRad + angleAmountRad;

		float clampedAngle = angleRad;
		if (endAngleRad <= Math::PI * 2)
		{
			clampedAngle = Math::clamp(angleRad, startAngleRad, endAngleRad);
		}
		else
		{
			if (angleRad >= startAngleRad)
				clampedAngle = Math::clamp(angleRad, startAngleRad, Math::PI * 2);
			else
			{
				endAngleRad -= Math::PI * 2;
				if (angleRad > endAngleRad)
				{
					if ((startAngleRad - angleRad) > (angleRad - endAngleRad))
						clampedAngle = endAngleRad;
					else
						clampedAngle = startAngleRad;
				}
				else
					clampedAngle = angleRad;
			}
		}

		Vector3 clampedAnglePoint;
		clampedAnglePoint.x = Math::cos(clampedAngle) * radius;
		clampedAnglePoint.z = Math::sin(clampedAngle) * radius;

		return worldToPlane.inverseAffine().multiplyAffine(clampedAnglePoint);
	}

	Degree HandleSliderDisc::pointOnCircleToAngle(Vector3 up, Vector3 point)
	{
		Vector3 arcBasis[3];
		arcBasis[1] = up;
		arcBasis[1].orthogonalComplement(arcBasis[2], arcBasis[0]);

		Matrix4 worldToPlane = Matrix4::IDENTITY;
		worldToPlane.setColumn(0, (Vector4)arcBasis[0]);
		worldToPlane.setColumn(1, (Vector4)arcBasis[1]);
		worldToPlane.setColumn(2, (Vector4)arcBasis[2]);

		point = worldToPlane.multiplyAffine(point);

		return Radian(Math::atan2(-point.z, -point.x) + Math::PI);
	}

	void HandleSliderDisc::activate(const CameraHandlerPtr& camera, const Vector2I& pointerPos)
	{
		Ray localRay = camera->screenPointToRay(pointerPos);
		localRay.transformAffine(getTransformInv());

		mStartPosition = calculateClosestPointOnArc(localRay, Vector3::ZERO, mNormal, mRadius, Degree(0.0f), Degree(360.0f));
		mStartAngle = pointOnCircleToAngle(mNormal, mStartPosition);
		mStartPosition = getTransform().multiplyAffine(mStartPosition);

		mDirection = mNormal.cross(mStartPosition - getPosition());
		mDirection.normalize();
	}

	void HandleSliderDisc::handleInput(const CameraHandlerPtr& camera, const Vector2I& inputDelta)
	{
		assert(getState() == State::Active);

		mCurrentPointerPos += inputDelta;
		mDelta = calcDelta(camera, mStartPosition, mDirection, mStartPointerPos, mCurrentPointerPos) * Math::RAD2DEG;
	}
}