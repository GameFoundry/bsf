#include "BsLight.h"
#include "BsLightRTTI.h"
#include "BsRendererManager.h"
#include "BsRenderer.h"
#include "BsFrameAlloc.h"
#include "BsSceneObject.h"

namespace BansheeEngine
{
	LightBase::LightBase()
		:mType(LightType::Point), mCastsShadows(false), mRange(10.0f),
		mIntensity(100.0f), mSpotAngle(45), mColor(Color::White), mIsActive(true)
	{
		mBounds = Sphere(mPosition, mRange);
	}

	LightBase::LightBase(LightType type, Color color,
		float intensity, float range, bool castsShadows, Degree spotAngle, Degree spotFalloffAngle)
		:mType(type), mCastsShadows(castsShadows), mRange(range), mSpotFalloffAngle(spotFalloffAngle),
		mIntensity(intensity), mSpotAngle(spotAngle), mColor(color), mIsActive(true)
	{
		mBounds = Sphere(mPosition, mRange);
	}

	float LightBase::getRadiance() const
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

	void LightBase::updateBounds()
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

	LightCore::LightCore(LightType type, Color color,
		float intensity, float range, bool castsShadows, Degree spotAngle, Degree spotFalloffAngle)
		:LightBase(type, color, intensity, range, castsShadows, spotAngle, spotFalloffAngle), mRendererId(0)
	{

	}

	LightCore::~LightCore()
	{
		gRenderer()->_notifyLightRemoved(this);
	}

	void LightCore::initialize()
	{
		gRenderer()->_notifyLightAdded(this);

		CoreObjectCore::initialize();
	}

	void LightCore::syncToCore(const CoreSyncData& data)
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

	Light::Light()
		:mLastUpdateHash(0)
	{
		
	}

	Light::Light(LightType type, Color color,
		float intensity, float range, bool castsShadows, Degree spotAngle, Degree spotFalloffAngle)
		: LightBase(type, color, intensity, range, castsShadows, spotAngle, spotFalloffAngle),
		mLastUpdateHash(0)
	{

	}

	SPtr<LightCore> Light::getCore() const
	{
		return std::static_pointer_cast<LightCore>(mCoreSpecific);
	}

	SPtr<Light> Light::create(LightType type, Color color,
		float intensity, float range, bool castsShadows, Degree spotAngle, Degree spotFalloffAngle)
	{
		Light* handler = new (bs_alloc<Light>()) 
			Light(type, color, intensity, range, castsShadows, spotAngle, spotFalloffAngle);
		SPtr<Light> handlerPtr = bs_core_ptr<Light>(handler);
		handlerPtr->_setThisPtr(handlerPtr);
		handlerPtr->initialize();

		return handlerPtr;
	}

	SPtr<Light> Light::createEmpty()
	{
		Light* handler = new (bs_alloc<Light>()) Light();
		SPtr<Light> handlerPtr = bs_core_ptr<Light>(handler);
		handlerPtr->_setThisPtr(handlerPtr);

		return handlerPtr;
	}

	SPtr<CoreObjectCore> Light::createCore() const
	{
		LightCore* handler = new (bs_alloc<LightCore>()) 
			LightCore(mType, mColor, mIntensity, mRange, mCastsShadows, mSpotAngle, mSpotFalloffAngle);
		SPtr<LightCore> handlerPtr = bs_shared_ptr<LightCore>(handler);
		handlerPtr->_setThisPtr(handlerPtr);

		return handlerPtr;
	}

	CoreSyncData Light::syncToCore(FrameAlloc* allocator)
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

	void Light::_updateTransform(const HSceneObject& parent)
	{
		UINT32 curHash = parent->getTransformHash();
		if (curHash != _getLastModifiedHash())
		{
			setPosition(parent->getWorldPosition());
			setRotation(parent->getWorldRotation());
			_setLastModifiedHash(curHash);
		}
	}

	void Light::_markCoreDirty(LightDirtyFlag flag)
	{
		markCoreDirty((UINT32)flag);
	}

	RTTITypeBase* Light::getRTTIStatic()
	{
		return LightRTTI::instance();
	}

	RTTITypeBase* Light::getRTTI() const
	{
		return Light::getRTTIStatic();
	}
}