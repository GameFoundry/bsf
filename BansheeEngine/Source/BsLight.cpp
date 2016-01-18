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
	/** Contains stencil meshes used for rendering light geometry */
	class LightStencilGeometry
	{
	public:
		/** Returns a stencil mesh used for a point light (a unit sphere). */
		static SPtr<MeshCore> getPointLightStencil()
		{
			if (sPointLightMesh == nullptr)
			{
				SPtr<VertexDataDesc> vertexDesc = bs_shared_ptr_new<VertexDataDesc>();
				vertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);

				UINT32 numVertices = 0;
				UINT32 numIndices = 0;

				ShapeMeshes3D::getNumElementsSphere(3, numVertices, numIndices);
				MeshDataPtr meshData = bs_shared_ptr_new<MeshData>(numVertices, numIndices, vertexDesc);

				UINT32* indexData = meshData->getIndices32();
				UINT8* positionData = meshData->getElementData(VES_POSITION);

				Sphere localSphere(Vector3::ZERO, 1.0f);
				ShapeMeshes3D::solidSphere(localSphere, positionData, nullptr, 0,
					vertexDesc->getVertexStride(), indexData, 0, 3);

				sPointLightMesh = MeshCore::create(meshData);
			}

			return sPointLightMesh;
		}

		/** Returns a stencil mesh used for spot light. Actual vertex positions need to be computed in shader. */
		static SPtr<MeshCore> getSpotLightStencil()
		{
			if (sSpotLightMesh == nullptr)
			{
				UINT32 numSides = LightCore::LIGHT_CONE_NUM_SIDES;
				UINT32 numSlices = LightCore::LIGHT_CONE_NUM_SLICES;

				SPtr<VertexDataDesc> vertexDesc = bs_shared_ptr_new<VertexDataDesc>();
				vertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);

				UINT32 numVertices = numSides * numSlices * 2;
				UINT32 numIndices = ((numSides * 2) * (numSlices - 1) * 2) * 3;

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
				for (UINT32 sliceIdx = 0; sliceIdx < (numSlices - 1); sliceIdx++)
				{
					for (UINT32 sideIdx = 0; sideIdx < numSides; sideIdx++)
					{
						indexData[curIdx++] = sliceIdx * numSides + sideIdx;
						indexData[curIdx++] = sliceIdx * numSides + (sideIdx + 1) % numSides;
						indexData[curIdx++] = (sliceIdx + 1) * numSides + sideIdx;

						indexData[curIdx++] = sliceIdx * numSides + (sideIdx + 1) % numSides;
						indexData[curIdx++] = (sliceIdx + 1) * numSides + (sideIdx + 1) % numSides;
						indexData[curIdx++] = (sliceIdx + 1) * numSides + sideIdx;
					}
				}

				// Sphere cap indices
				UINT32 coneOffset = numSides * numSlices;
				for (UINT32 sliceIdx = 0; sliceIdx < (numSlices - 1); sliceIdx++)
				{
					for (UINT32 sideIdx = 0; sideIdx < numSides; sideIdx++)
					{
						indexData[curIdx++] = coneOffset + sliceIdx * numSides + sideIdx;
						indexData[curIdx++] = coneOffset + sliceIdx * numSides + (sideIdx + 1) % numSides;
						indexData[curIdx++] = coneOffset + (sliceIdx + 1) * numSides + sideIdx;

						indexData[curIdx++] = coneOffset + sliceIdx * numSides + (sideIdx + 1) % numSides;
						indexData[curIdx++] = coneOffset + (sliceIdx + 1) * numSides + (sideIdx + 1) % numSides;
						indexData[curIdx++] = coneOffset + (sliceIdx + 1) * numSides + sideIdx;
					}
				}

				sSpotLightMesh = MeshCore::create(meshData);
			}

			return sSpotLightMesh;
		}

	private:
		static SPtr<MeshCore> sPointLightMesh;
		static SPtr<MeshCore> sSpotLightMesh;
	};

	SPtr<MeshCore> LightStencilGeometry::sPointLightMesh;
	SPtr<MeshCore> LightStencilGeometry::sSpotLightMesh;

	LightBase::LightBase()
		:mType(LightType::Point), mCastsShadows(false), mRange(10.0f),
		mIntensity(5.0f), mSpotAngle(45), mSpotFalloffAngle(35.0f), mColor(Color::White), 
		mIsActive(true), mPhysCorrectAtten(true)
	{
		updatePhysicallyCorrectRange();
	}

	LightBase::LightBase(LightType type, Color color,
		float intensity, float range, bool castsShadows, Degree spotAngle, Degree spotFalloffAngle)
		:mType(type), mCastsShadows(castsShadows), mRange(range), mSpotFalloffAngle(spotFalloffAngle),
		mIntensity(intensity), mSpotAngle(spotAngle), mColor(color), mIsActive(true), mPhysCorrectAtten(true)
	{
		updatePhysicallyCorrectRange();
	}

	void LightBase::setPhysicallyBasedAttenuation(bool enabled)
	{
		mPhysCorrectAtten = enabled; 

		if(enabled)
			updatePhysicallyCorrectRange();

		_markCoreDirty();
	}

	void LightBase::setRange(float range)
	{
		if (mPhysCorrectAtten)
			return;

		mRange = range; 
		_markCoreDirty(); 
		updateBounds();
	}

	void LightBase::setIntensity(float intensity)
	{
		mIntensity = intensity; 

		if (mPhysCorrectAtten)
			updatePhysicallyCorrectRange();

		_markCoreDirty(); 
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

	void LightBase::updatePhysicallyCorrectRange()
	{
		// When lower than this attenuation light influence is assumed to be zero
		const float minAttenuation = 0.05f;
		mRange = sqrt(mIntensity / minAttenuation - 1.0f);

		updateBounds();
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
		dataPtr = rttiReadElem(mPhysCorrectAtten, dataPtr);
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

	SPtr<MeshCore> LightCore::getMesh() const
	{
		switch (mType)
		{
		case LightType::Directional:
			return nullptr;
		case LightType::Point:
			return LightStencilGeometry::getPointLightStencil();
		case LightType::Spot:
			return LightStencilGeometry::getSpotLightStencil();
		}

		return nullptr;
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
		size += rttiGetElemSize(mSpotFalloffAngle);
		size += rttiGetElemSize(mPhysCorrectAtten);
		size += rttiGetElemSize(mIsActive);
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
		dataPtr = rttiWriteElem(mPhysCorrectAtten, dataPtr);
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