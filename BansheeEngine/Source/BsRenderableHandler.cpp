#include "BsRenderableHandler.h"
#include "BsRenderableHandlerRTTI.h"
#include "BsSceneObject.h"
#include "BsBuiltinResources.h"
#include "BsMesh.h"
#include "BsMaterial.h"
#include "BsRenderQueue.h"
#include "BsBounds.h"
#include "BsRenderer.h"
#include "BsFrameAlloc.h"

namespace BansheeEngine
{
	template<bool Core>
	TRenderableHandler<Core>::TRenderableHandler()
		:mLayer(1), mTransform(Matrix4::IDENTITY), mIsActive(true)
	{
		mMaterials.resize(1);
	}

	template<bool Core>
	TRenderableHandler<Core>::~TRenderableHandler()
	{
		int a = 5;
	}

	template<bool Core>
	void TRenderableHandler<Core>::setMesh(const MeshType& mesh)
	{
		mMesh = mesh;
		mMaterials.resize(mesh->getProperties().getNumSubMeshes());

		_markResourcesDirty();
		_markCoreDirty();
	}

	template<bool Core>
	void TRenderableHandler<Core>::setMaterial(UINT32 idx, const MaterialType& material)
	{
		mMaterials[idx] = material;

		_markResourcesDirty();
		_markCoreDirty();
	}

	template<bool Core>
	void TRenderableHandler<Core>::setMaterial(const MaterialType& material)
	{
		setMaterial(0, material);
	}

	template<bool Core>
	void TRenderableHandler<Core>::setLayer(UINT64 layer)
	{
		bool isPow2 = layer && !((layer - 1) & layer);

		if (!isPow2)
			BS_EXCEPT(InvalidParametersException, "Invalid layer provided. Only one layer bit may be set.");

		mLayer = layer;
		_markCoreDirty();
	}

	template<bool Core>
	void TRenderableHandler<Core>::setTransform(const Matrix4& transform)
	{
		mTransform = transform;
		_markCoreDirty(RenderableDirtyFlag::Transform);
	}

	template<bool Core>
	void TRenderableHandler<Core>::setPosition(const Vector3& position)
	{
		mPosition = position;
		_markCoreDirty(RenderableDirtyFlag::Transform);
	}

	template<bool Core>
	void TRenderableHandler<Core>::setIsActive(bool active)
	{
		mIsActive = active;
		_markCoreDirty();
	}

	template class TRenderableHandler < false >;
	template class TRenderableHandler < true >;

	RenderableHandlerCore::RenderableHandlerCore() 
		:mRendererId(0)
	{
	}

	RenderableHandlerCore::~RenderableHandlerCore()
	{
		if (mIsActive)
			gRenderer()->_notifyRenderableRemoved(this);
	}

	void RenderableHandlerCore::initialize()
	{
		gRenderer()->_notifyRenderableAdded(this);

		CoreObjectCore::initialize();
	}

	Bounds RenderableHandlerCore::getBounds() const
	{
		SPtr<MeshCore> mesh = getMesh();

		if (mesh == nullptr)
		{
			AABox box(mPosition, mPosition);
			Sphere sphere(mPosition, 0.0f);

			return Bounds(box, sphere);
		}
		else
		{
			Bounds bounds = mesh->getProperties().getBounds();
			bounds.transformAffine(mTransform);

			return bounds;
		}
	}

	void RenderableHandlerCore::syncToCore(const CoreSyncData& data)
	{
		char* dataPtr = (char*)data.getBuffer();

		mWorldBounds.clear();
		mMaterials.clear();

		UINT32 numMaterials = 0;
		UINT32 dirtyFlags = 0;
		bool oldIsActive = mIsActive;

		dataPtr = rttiReadElem(mLayer, dataPtr);
		dataPtr = rttiReadElem(mWorldBounds, dataPtr);
		dataPtr = rttiReadElem(numMaterials, dataPtr);
		dataPtr = rttiReadElem(mTransform, dataPtr);
		dataPtr = rttiReadElem(mPosition, dataPtr);
		dataPtr = rttiReadElem(mIsActive, dataPtr);
		dataPtr = rttiReadElem(dirtyFlags, dataPtr);

		SPtr<MeshCore>* mesh = (SPtr<MeshCore>*)dataPtr;
		mMesh = *mesh;
		mesh->~SPtr<MeshCore>();
		dataPtr += sizeof(SPtr<MeshCore>);

		for (UINT32 i = 0; i < numMaterials; i++)
		{
			SPtr<MaterialCore>* material = (SPtr<MaterialCore>*)dataPtr;
			mMaterials.push_back(*material);
			material->~SPtr<MaterialCore>();
			dataPtr += sizeof(SPtr<MaterialCore>);
		}

		if (dirtyFlags == (UINT32)RenderableDirtyFlag::Transform)
		{
			if (mIsActive)
				gRenderer()->_notifyRenderableUpdated(this);
		}
		else
		{
			if (oldIsActive != mIsActive)
			{
				if (mIsActive)
					gRenderer()->_notifyRenderableAdded(this);
				else
					gRenderer()->_notifyRenderableRemoved(this);
			}
			else
			{
				gRenderer()->_notifyRenderableRemoved(this);
				gRenderer()->_notifyRenderableAdded(this);
			}
		}
	}

	Bounds RenderableHandler::getBounds() const
	{
		HMesh mesh = getMesh();

		if (!mesh.isLoaded())
		{
			AABox box(mPosition, mPosition);
			Sphere sphere(mPosition, 0.0f);

			return Bounds(box, sphere);
		}
		else
		{
			Bounds bounds = mesh->getProperties().getBounds();
			bounds.transformAffine(mTransform);

			return bounds;
		}
	}

	SPtr<RenderableHandlerCore> RenderableHandler::getCore() const
	{
		return std::static_pointer_cast<RenderableHandlerCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> RenderableHandler::createCore() const
	{
		RenderableHandlerCore* handler = new (bs_alloc<RenderableHandlerCore>()) RenderableHandlerCore();
		SPtr<RenderableHandlerCore> handlerPtr = bs_shared_ptr<RenderableHandlerCore, GenAlloc>(handler);
		handlerPtr->_setThisPtr(handlerPtr);

		return handlerPtr;
	}

	void RenderableHandler::_markCoreDirty(RenderableDirtyFlag flag)
	{
		markCoreDirty((UINT32)flag);
	}

	void RenderableHandler::_markResourcesDirty()
	{
		markResourcesDirty();
	}

	CoreSyncData RenderableHandler::syncToCore(FrameAlloc* allocator)
	{
		UINT32 numMaterials = (UINT32)mMaterials.size();

		UINT32 size = rttiGetElemSize(mLayer) + 
			rttiGetElemSize(mWorldBounds) + 
			rttiGetElemSize(numMaterials) + 
			rttiGetElemSize(mTransform) +
			rttiGetElemSize(mPosition) +
			rttiGetElemSize(mIsActive) +
			rttiGetElemSize(getCoreDirtyFlags()) +
			sizeof(SPtr<MeshCore>) + 
			numMaterials * sizeof(SPtr<MaterialCore>);

		UINT8* data = allocator->alloc(size);
		char* dataPtr = (char*)data;
		dataPtr = rttiWriteElem(mLayer, dataPtr);
		dataPtr = rttiWriteElem(mWorldBounds, dataPtr);
		dataPtr = rttiWriteElem(numMaterials, dataPtr);
		dataPtr = rttiWriteElem(mTransform, dataPtr);
		dataPtr = rttiWriteElem(mPosition, dataPtr);
		dataPtr = rttiWriteElem(mIsActive, dataPtr);
		dataPtr = rttiWriteElem(getCoreDirtyFlags(), dataPtr);

		SPtr<MeshCore>* mesh = new (dataPtr) SPtr<MeshCore>();
		if (mMesh.isLoaded())
			*mesh = mMesh->getCore();

		dataPtr += sizeof(SPtr<MeshCore>);

		for (UINT32 i = 0; i < numMaterials; i++)
		{
			SPtr<MaterialCore>* material = new (dataPtr)SPtr<MaterialCore>();
			if (mMaterials[i].isLoaded())
				*material = mMaterials[i]->getCore();

			dataPtr += sizeof(SPtr<MaterialCore>);
		}

		return CoreSyncData(data, size);
	}

	void RenderableHandler::getResourceDependencies(Vector<HResource>& resources)
	{
		if (mMesh != nullptr)
			resources.push_back(mMesh);

		for (auto& material : mMaterials)
		{
			if (material != nullptr)
				resources.push_back(material);
		}
	}

	RenderableHandlerPtr RenderableHandler::create()
	{
		RenderableHandlerPtr handlerPtr = createEmpty();
		handlerPtr->initialize();

		return handlerPtr;
	}

	RenderableHandlerPtr RenderableHandler::createEmpty()
	{
		RenderableHandler* handler = new (bs_alloc<RenderableHandler>()) RenderableHandler();
		RenderableHandlerPtr handlerPtr = bs_core_ptr<RenderableHandler, GenAlloc>(handler);
		handlerPtr->_setThisPtr(handlerPtr);

		return handlerPtr;
	}

	RTTITypeBase* RenderableHandler::getRTTIStatic()
	{
		return RenderableHandlerRTTI::instance();
	}

	RTTITypeBase* RenderableHandler::getRTTI() const
	{
		return RenderableHandler::getRTTIStatic();
	}
}