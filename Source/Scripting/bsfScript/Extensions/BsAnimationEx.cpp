//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Extensions/BsAnimationEx.h"

using namespace std::placeholders;

namespace bs
{
	Vector<TNamedAnimationCurve<Vector3>> AnimationCurvesEx::getPositionCurves(const SPtr<AnimationCurves>& thisPtr)
	{
		return thisPtr->position;
	}

	void AnimationCurvesEx::setPositionCurves(const SPtr<AnimationCurves>& thisPtr, const Vector<TNamedAnimationCurve<Vector3>>& value)
	{
		thisPtr->position = value;
	}

	Vector<TNamedAnimationCurve<Quaternion>> AnimationCurvesEx::getRotationCurves(const SPtr<AnimationCurves>& thisPtr)
	{
		return thisPtr->rotation;
	}

	void AnimationCurvesEx::setRotationCurves(const SPtr<AnimationCurves>& thisPtr, const Vector<TNamedAnimationCurve<Quaternion>>& value)
	{
		thisPtr->rotation = value;
	}

	Vector<TNamedAnimationCurve<Vector3>> AnimationCurvesEx::getScaleCurves(const SPtr<AnimationCurves>& thisPtr)
	{
		return thisPtr->scale;
	}

	void AnimationCurvesEx::setScaleCurves(const SPtr<AnimationCurves>& thisPtr, const Vector<TNamedAnimationCurve<Vector3>>& value)
	{
		thisPtr->scale = value;
	}

	Vector<TNamedAnimationCurve<float>> AnimationCurvesEx::getGenericCurves(const SPtr<AnimationCurves>& thisPtr)
	{
		return thisPtr->generic;
	}

	void AnimationCurvesEx::setGenericCurves(const SPtr<AnimationCurves>& thisPtr, const Vector<TNamedAnimationCurve<float>>& value)
	{
		thisPtr->generic = value;
	}

	TAnimationCurve<Vector3> RootMotionEx::getPositionCurves(const SPtr<RootMotion>& thisPtr)
	{
		return thisPtr->position;
	}

	TAnimationCurve<Quaternion> RootMotionEx::getRotationCurves(const SPtr<RootMotion>& thisPtr)
	{
		return thisPtr->rotation;
	}
}
