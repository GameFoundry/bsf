#include "BsLightInternal.h"
#include "BsRendererManager.h"
#include "BsRenderer.h"
#include "BsFrameAlloc.h"
#include "BsLightInternalRTTI.h"

namespace BansheeEngine
{
	LightInternalBase::LightInternalBase()
		:mType(LightType::Point), mCastsShadows(false), mRange(10.0f),
		mLuminousFlux(100.0f), mSpotAngle(45), mColor(Color::White)
	{
		
	}

	LightInternalBase::LightInternalBase(LightType type, Color color,
		float luminousFlux, float range, bool castsShadows, Degree spotAngle)
		:mType(type), mCastsShadows(castsShadows), mRange(range),
		mLuminousFlux(luminousFlux), mSpotAngle(spotAngle), mColor(color)
	{
		
	}

	LightInternalCore::~LightInternalCore()
	{
		RendererManager::instance().getActive()->_notifyLightRemoved(this);
	}

	LightInternalCore::LightInternalCore(LightType type, Color color,
		float luminousFlux, float range, bool castsShadows, Degree spotAngle)
		:LightInternalBase(type, color, luminousFlux, range, castsShadows, spotAngle)
	{

	}

	void LightInternalCore::initialize()
	{
		RendererManager::instance().getActive()->_notifyLightAdded(this);

		CoreObjectCore::initialize();
	}

	void LightInternalCore::syncToCore(const CoreSyncData& data)
	{
		char* dataPtr = (char*)data.getBuffer();

		dataPtr = rttiReadElem(mPosition, dataPtr);
		dataPtr = rttiReadElem(mRotation, dataPtr);
		dataPtr = rttiReadElem(mType, dataPtr);
		dataPtr = rttiReadElem(mCastsShadows, dataPtr);
		dataPtr = rttiReadElem(mColor, dataPtr);
		dataPtr = rttiReadElem(mRange, dataPtr);
		dataPtr = rttiReadElem(mLuminousFlux, dataPtr);
		dataPtr = rttiReadElem(mSpotAngle, dataPtr);
	}

	LightInternal::LightInternal(LightType type, Color color,
		float luminousFlux, float range, bool castsShadows, Degree spotAngle)
		:LightInternalBase(type, color, luminousFlux, range, castsShadows, spotAngle)
	{

	}

	SPtr<LightInternalCore> LightInternal::getCore() const
	{
		return std::static_pointer_cast<LightInternalCore>(mCoreSpecific);
	}

	SPtr<LightInternal> LightInternal::create(LightType type, Color color,
		float luminousFlux, float range, bool castsShadows, Degree spotAngle)
	{
		LightInternal* handler = new (bs_alloc<LightInternal>()) 
			LightInternal(type, color, luminousFlux, range, castsShadows, spotAngle);
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
			LightInternalCore(mType, mColor, mLuminousFlux, mRange, mCastsShadows, mSpotAngle);
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
		size += rttiGetElemSize(mLuminousFlux);
		size += rttiGetElemSize(mSpotAngle);

		UINT8* buffer = allocator->alloc(size);

		char* dataPtr = (char*)buffer;
		dataPtr = rttiWriteElem(mPosition, dataPtr);
		dataPtr = rttiWriteElem(mRotation, dataPtr);
		dataPtr = rttiWriteElem(mType, dataPtr);
		dataPtr = rttiWriteElem(mCastsShadows, dataPtr);
		dataPtr = rttiWriteElem(mColor, dataPtr);
		dataPtr = rttiWriteElem(mRange, dataPtr);
		dataPtr = rttiWriteElem(mLuminousFlux, dataPtr);
		dataPtr = rttiWriteElem(mSpotAngle, dataPtr);

		return CoreSyncData(buffer, size);
	}

	void LightInternal::_markCoreDirty()
	{
		markCoreDirty();
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