#include "BsLight.h"
#include "BsLightRTTI.h"
#include "BsRendererManager.h"
#include "BsRenderer.h"
#include "BsFrameAlloc.h"
#include "BsSceneObject.h"
#include "BsVertexDataDesc.h"
#include "BsMesh.h"
#include "BsShapeMeshes3D.h"

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

	const UINT32 LightCore::LIGHT_CONE_NUM_SIDES = 20;
	const UINT32 LightCore::LIGHT_CONE_NUM_SLICES = 10;

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
		updateBounds();
		gRenderer()->_notifyLightAdded(this);

		CoreObjectCore::initialize();
	}

	void LightCore::syncToCore(const CoreSyncData& data)
	{
		char* dataPtr = (char*)data.getBuffer();

		UINT32 dirtyFlags = 0;
		bool oldIsActive = mIsActive;
		LightType oldType = mType;

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

		updateBounds();

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
				{
					LightType newType = mType;
					mType = oldType;
					gRenderer()->_notifyLightRemoved(this);
					mType = newType;
				}
			}
			else
			{
				LightType newType = mType;
				mType = oldType;
				gRenderer()->_notifyLightRemoved(this);
				mType = newType;

				gRenderer()->_notifyLightAdded(this);
			}
		}
	}

	void LightCore::updateBounds()
	{
		LightBase::updateBounds();

		generateMesh();
	}

	void LightCore::generateMesh()
	{
		switch (mType)
		{
		case LightType::Directional:
			mMesh = nullptr;
			return;
		case LightType::Point:
		{
			SPtr<VertexDataDesc> vertexDesc = bs_shared_ptr_new<VertexDataDesc>();
			vertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);

			UINT32 numVertices = 0;
			UINT32 numIndices = 0;

			ShapeMeshes3D::getNumElementsSphere(1, numVertices, numIndices);
			MeshDataPtr meshData = bs_shared_ptr_new<MeshData>(numVertices, numIndices, vertexDesc);

			UINT32* indexData = meshData->getIndices32();
			UINT8* positionData = meshData->getElementData(VES_POSITION);

			ShapeMeshes3D::solidSphere(mBounds, positionData, nullptr, 0,
				vertexDesc->getVertexStride(), indexData, 0, 1);

			mMesh = MeshCore::create(meshData);
		}
			return;
		case LightType::Spot:
		{
			SPtr<VertexDataDesc> vertexDesc = bs_shared_ptr_new<VertexDataDesc>();
			vertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);

			UINT32 numVertices = LIGHT_CONE_NUM_SIDES * LIGHT_CONE_NUM_SLICES * 2;
			UINT32 numIndices = ((LIGHT_CONE_NUM_SIDES * 2) * LIGHT_CONE_NUM_SLICES * 2) * 3;

			ShapeMeshes3D::getNumElementsCone(1, numVertices, numIndices);
			MeshDataPtr meshData = bs_shared_ptr_new<MeshData>(numVertices, numIndices, vertexDesc);

			UINT32* indexData = meshData->getIndices32();
			UINT8* positionData = meshData->getElementData(VES_POSITION);
			UINT32 stride = vertexDesc->getVertexStride();

			// Dummy vertex positions, actual ones generated in shader
			for (UINT32 i = 0; i < numVertices; i++)
			{
				memcpy(positionData, &Vector3::ZERO, sizeof(Vector3));
				positionData += stride;
			}

			// Cone indices
			UINT32 curIdx = 0;
			for (UINT32 sliceIdx = 0; sliceIdx < (LIGHT_CONE_NUM_SLICES - 1); sliceIdx++)
			{
				for (UINT32 sideIdx = 0; sideIdx < LIGHT_CONE_NUM_SIDES; sideIdx++)
				{
					indexData[curIdx++] = sliceIdx * LIGHT_CONE_NUM_SIDES + sideIdx;
					indexData[curIdx++] = sliceIdx * LIGHT_CONE_NUM_SIDES + (sideIdx + 1) % LIGHT_CONE_NUM_SIDES;
					indexData[curIdx++] = (sliceIdx + 1) * LIGHT_CONE_NUM_SIDES + sideIdx;

					indexData[curIdx++] = sliceIdx * LIGHT_CONE_NUM_SIDES + (sideIdx + 1) % LIGHT_CONE_NUM_SIDES;
					indexData[curIdx++] = (sliceIdx + 1) * LIGHT_CONE_NUM_SIDES + (sideIdx + 1) % LIGHT_CONE_NUM_SIDES;
					indexData[curIdx++] = (sliceIdx + 1) * LIGHT_CONE_NUM_SIDES + sideIdx;
				}
			}

			// Sphere cap indices
			UINT32 coneOffset = LIGHT_CONE_NUM_SIDES * LIGHT_CONE_NUM_SLICES;
			for (UINT32 sliceIdx = 0; sliceIdx < (LIGHT_CONE_NUM_SLICES - 1); sliceIdx++)
			{
				for (UINT32 sideIdx = 0; sideIdx < LIGHT_CONE_NUM_SIDES; sideIdx++)
				{
					indexData[curIdx++] = coneOffset + sliceIdx * LIGHT_CONE_NUM_SIDES + sideIdx;
					indexData[curIdx++] = coneOffset + sliceIdx * LIGHT_CONE_NUM_SIDES + (sideIdx + 1) % LIGHT_CONE_NUM_SIDES;
					indexData[curIdx++] = coneOffset + (sliceIdx + 1) * LIGHT_CONE_NUM_SIDES + sideIdx;

					indexData[curIdx++] = coneOffset + sliceIdx * LIGHT_CONE_NUM_SIDES + (sideIdx + 1) % LIGHT_CONE_NUM_SIDES;
					indexData[curIdx++] = coneOffset + (sliceIdx + 1) * LIGHT_CONE_NUM_SIDES + (sideIdx + 1) % LIGHT_CONE_NUM_SIDES;
					indexData[curIdx++] = coneOffset + (sliceIdx + 1) * LIGHT_CONE_NUM_SIDES + sideIdx;
				}
			}

			mMesh = MeshCore::create(meshData);
		}
			return;
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
		// Calling virtual method is okay here because this is the most derived type
		updateBounds();
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