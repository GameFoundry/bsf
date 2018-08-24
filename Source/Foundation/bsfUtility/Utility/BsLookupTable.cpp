//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsLookupTable.h"
#include "Math/BsMath.h"

namespace bs
{
	LookupTable::LookupTable(Vector<float> values, float startTime, float endTime, uint32_t sampleSize)
		: mValues(std::move(values))
		, mSampleSize(std::max(sampleSize, 1U))
		, mNumSamples((uint32_t)mValues.size() / mSampleSize)
		, mTimeStart(startTime)
	{
		if(endTime < startTime)
			endTime = startTime;

		float timeInterval;
		if(mNumSamples > 1)
			timeInterval = (endTime - startTime) / (mNumSamples - 1);
		else
			timeInterval = 0.0f;

		mTimeScale = 1.0f / timeInterval;
	}

	void LookupTable::evaluate(float t, float*& left, float*& right, float& fraction)
	{
		t -= mTimeStart;
		t *= mTimeScale;

		const auto index = (UINT32)t;
		fraction =  Math::frac(t);

		const UINT32 leftIdx = std::min(index, mNumSamples - 1);
		const UINT32 rightIdx = std::min(index + 1, mNumSamples - 1);

		left = &mValues[leftIdx * mSampleSize];
		right = &mValues[rightIdx * mSampleSize];
	}
}