#include "BsLightInternal.h"
#include "BsRendererManager.h"
#include "BsRenderer.h"
#include "BsFrameAlloc.h"
#include "BsSceneObject.h"
#include "BsLightInternalRTTI.h"

namespace BansheeEngine
{
	LightInternalBase::LightInternalBase()
		:mType(LightType::Point), mCastsShadows(false), mRange(10.0f),
		mIntensity(100.0f), mSpotAngle(45), mColor(Color::White), mIsActive(true)
	{
		mBounds = Sphere(mPosition, mRange);
	}

	LightInternalBase::LightInternalBase(LightType type, Color color,
		float intensity, float range, bool castsShadows, Degree spotAngle, Degree spotFalloffAngle)
		:mType(type), mCastsShadows(castsShadows), mRange(range), mSpotFalloffAngle(spotFalloffAngle),
		mIntensity(intensity), mSpotAngle(spotAngle), mColor(color), mIsActive(true)
	{
		mBounds = Sphere(mPosition, mRange);
	}

	float LightInternalBase::getRadiance() const
	{
		switch (mType)
		{
		case LightType::Point:
			return mIntensity / (4 * Math::PI);
		case LightType::Spot:
		{
			float cosTotalAngle = Math::cos(mSpotAngle);
			float cosFalloffAngle = Math::cos(mSpotFalloffAngle);

			return mIntensity / (Math::TWO_PI * (1.0f - (cosFalloffAngle + cosTotalAngle) * 0.5f));
		}
		}

		return mIntensity;
	}

	void LightInternalBase::updateBounds()
	{
		switch (mType)
		{
		case LightType::Directional:
			mBounds = Sphere(mPosition, std::numeric_limits<float>::infinity());
			break;
		case LightType::Point:
			mBounds = Sphere(mPosition, mRange);
			break;
		case LightType::Spot:
		{
			Degree angle = Math::clamp(mSpotAngle, Degree(-90), Degree(90));
			float coneRadius = Math::tan(angle) * mRange;

			float radius;
			Vector3 offset;
			if (coneRadius < mRange)
			{
				radius = (mRange * mRange + coneRadius * coneRadius) / (0.5f * mRange);
				offset = Vector3(0, 0, -(mRange - coneRadius));
			}
			else
				radius = coneRadius;

			Vector3 center = mPosition + mRotation.rotate(offset);
			mBounds = Sphere(center, radius);
		}
			break;
		}
	}

	LightInternalCore::~LightInternalCore()
	{
		gRenderer()->_notifyLightRemoved(this);
	}

	LightInternalCore::LightInternalCore(LightType type, Color color,
		float intensity, float range, bool castsShadows, Degree spotAngle, Degree spotFalloffAngle)
		:LightInternalBase(type, color, intensity, range, castsShadows, spotAngle, spotFalloffAngle)
	{

	}

	void LightInternalCore::initialize()
	{
		gRenderer()->_notifyLightAdded(this);

		CoreObjectCore::initialize();
	}

	void LightInternalCore::syncToCore(const CoreSyncData& data)
	{
		char* dataPtr = (char*)data.getBuffer();

		UINT32 dirtyFlags = 0;
		bool oldIsActive = mIsActive;

		dataPtr = rttiReadElem(mPosition, dataPtr);
		dataPtr = rttiReadElem(mRotation, dataPtr);
		dataPtr = rttiReadElem(mType, dataPtr);
		dataPtr = rttiReadElem(mCastsShadows, dataPtr);
		dataPtr = rttiReadElem(mColor, dataPtr);
		dataPtr = rttiReadElem(mRange, dataPtr);
		dataPtr = rttiReadElem(mIntensity, dataPtr);
		dataPtr = rttiReadElem(mSpotAngle, dataPtr);
		dataPtr = rttiReadElem(mSpotFalloffAngle, dataPtr);
		dataPtr = rttiReadElem(mIsActive, dataPtr);
		dataPtr = rttiReadElem(dirtyFlags, dataPtr);
		dataPtr = rttiReadElem(mBounds, dataPtr);

		if (dirtyFlags == (UINT32)LightDirtyFlag::Transform)
		{
			if (mIsActive)
				gRenderer()->_notifyLightUpdated(this);
		}
		else
		{
			if (oldIsActive != mIsActive)
			{
				if (mIsActive)
					gRenderer()->_notifyLightAdded(this);
				else
					gRenderer()->_notifyLightRemoved(this);
			}
			else
			{
				gRenderer()->_notifyLightRemoved(this);
				gRenderer()->_notifyLightAdded(this);
			}
		}
	}

	LightInternal::LightInternal()
		:mLastUpdateHash(0)
	{
		
	}

	LightInternal::LightInternal(LightType type, Color color,
		float intensity, float range, bool castsShadows, Degree spotAngle, Degree spotFalloffAngle)
		: LightInternalBase(type, color, intensity, range, castsShadows, spotAngle, spotFalloffAngle),
		mLastUpdateHash(0)
	{

	}

	SPtr<LightInternalCore> LightInternal::getCore() const
	{
		return std::static_pointer_cast<LightInternalCore>(mCoreSpecific);
	}

	SPtr<LightInternal> LightInternal::create(LightType type, Color color,
		float intensity, float range, bool castsShadows, Degree spotAngle, Degree spotFalloffAngle)
	{
		LightInternal* handler = new (bs_alloc<LightInternal>()) 
			LightInternal(type, color, intensity, range, castsShadows, spotAngle, spotFalloffAngle);
		SPtr<LightInternal> handlerPtr = bs_core_ptr<LightInternal, GenAlloc>(handler);
		handlerPtr->_setThisPtr(handlerPtr);
		handlerPtr->initialize();

		return handlerPtr;
	}

	SPtr<LightInternal> LightInternal::createEmpty()
	{
		LightInternal* handler = new (bs_alloc<LightInternal>()) LightInternal();
		SPtr<LightInternal> handlerPtr = bs_core_ptr<LightInternal, GenAlloc>(handler);
		handlerPtr->_setThisPtr(handlerPtr);

		return handlerPtr;
	}

	SPtr<CoreObjectCore> LightInternal::createCore() const
	{
		LightInternalCore* handler = new (bs_alloc<LightInternalCore>()) 
			LightInternalCore(mType, mColor, mIntensity, mRange, mCastsShadows, mSpotAngle, mSpotFalloffAngle);
		SPtr<LightInternalCore> handlerPtr = bs_shared_ptr<LightInternalCore, GenAlloc>(handler);
		handlerPtr->_setThisPtr(handlerPtr);

		return handlerPtr;
	}

	CoreSyncData LightInternal::syncToCore(FrameAlloc* allocator)
	{
		UINT32 size = 0;
		size += rttiGetElemSize(mPosition);
		size += rttiGetElemSize(mRotation);
		size += rttiGetElemSize(mType);
		size += rttiGetElemSize(mCastsShadows);
		size += rttiGetElemSize(mColor);
		size += rttiGetElemSize(mRange);
		size += rttiGetElemSize(mIntensity);
		size += rttiGetElemSize(mSpotAngle);
		size += rttiGetElemSize(mIsActive);
		size += rttiGetElemSize(mSpotFalloffAngle);
		size += rttiGetElemSize(getCoreDirtyFlags());
		size += rttiGetElemSize(mBounds);

		UINT8* buffer = allocator->alloc(size);

		char* dataPtr = (char*)buffer;
		dataPtr = rttiWriteElem(mPosition, dataPtr);
		dataPtr = rttiWriteElem(mRotation, dataPtr);
		dataPtr = rttiWriteElem(mType, dataPtr);
		dataPtr = rttiWriteElem(mCastsShadows, dataPtr);
		dataPtr = rttiWriteElem(mColor, dataPtr);
		dataPtr = rttiWriteElem(mRange, dataPtr);
		dataPtr = rttiWriteElem(mIntensity, dataPtr);
		dataPtr = rttiWriteElem(mSpotAngle, dataPtr);
		dataPtr = rttiWriteElem(mSpotFalloffAngle, dataPtr);
		dataPtr = rttiWriteElem(mIsActive, dataPtr);
		dataPtr = rttiWriteElem(getCoreDirtyFlags(), dataPtr);
		dataPtr = rttiWriteElem(mBounds, dataPtr);

		return CoreSyncData(buffer, size);
	}

	void LightInternal::_updateTransform(const HSceneObject& parent)
	{
		UINT32 curHash = parent->getTransformHash();
		if (curHash != _getLastModifiedHash())
		{
			setPosition(parent->getWorldPosition());
			setRotation(parent->getWorldRotation());
			_setLastModifiedHash(curHash);
		}
	}

	void LightInternal::_markCoreDirty(LightDirtyFlag flag = LightDirtyFlag::Everything)
	{
		markCoreDirty((UINT32)flag);
	}

	RTTITypeBase* LightInternal::getRTTIStatic()
	{
		return LightInternalRTTI::instance();
	}

	RTTITypeBase* LightInternal::getRTTI() const
	{
		return LightInternal::getRTTIStatic();
	}
}