#include "BsRenderable.h"
#include "BsRenderableRTTI.h"
#include "BsSceneObject.h"
#include "BsBuiltinMaterialManager.h"
#include "BsMesh.h"
#include "BsMaterial.h"
#include "BsRenderQueue.h"

namespace BansheeEngine
{
	Renderable::MeshData::MeshData(const HMesh& mesh)
	{
		this->mesh = mesh;
		isLoaded = mesh.isLoaded();
	}

	Renderable::MaterialData::MaterialData(const HMaterial& material)
	{
		this->material = material;
		isLoaded = material.isLoaded();
	}

	Renderable::Renderable(const HSceneObject& parent)
		:Component(parent), mLayer(1), mIsRenderDataDirty(true), mActiveProxy(nullptr)
	{
		setName("Renderable");

		mMaterialData.resize(1);
	}

	void Renderable::setMesh(HMesh mesh)
	{
		mMeshData = mesh;

		mIsRenderDataDirty = true;
	}

	void Renderable::setNumMaterials(UINT32 numMaterials)
	{
		numMaterials = std::max(1U, numMaterials);

		mMaterialData.resize(numMaterials);

		mIsRenderDataDirty = true;
	}

	void Renderable::setMaterial(UINT32 idx, HMaterial material)
	{
		mMaterialData[idx] = material;

		mIsRenderDataDirty = true;
	}

	void Renderable::setMaterial(HMaterial material)
	{
		setMaterial(0, material);
	}

	void Renderable::setLayer(UINT64 layer)
	{
		bool isPow2 = layer && !((layer - 1) & layer);

		if (!isPow2)
			BS_EXCEPT(InvalidParametersException, "Invalid layer provided. Only one layer bit may be set.");

		mLayer = layer;
		mIsRenderDataDirty = true;
	}

	bool Renderable::_isRenderDataDirty() const
	{ 
		updateResourceLoadStates();

		return mIsRenderDataDirty; 
	}

	void Renderable::updateResourceLoadStates() const
	{
		if (!mMeshData.isLoaded && mMeshData.mesh != nullptr && mMeshData.mesh.isLoaded())
		{
			mIsRenderDataDirty = true;
			mMeshData.isLoaded = true;
		}

		for (auto& materialData : mMaterialData)
		{
			if (!materialData.isLoaded && materialData.material != nullptr && materialData.material.isLoaded())
			{
				mIsRenderDataDirty = true;
				materialData.isLoaded = true;
			}
		}
	}

	RenderableProxy* Renderable::_createProxy(FrameAlloc* allocator) const
	{
		if (mMeshData.mesh == nullptr || !mMeshData.mesh.isLoaded())
			return nullptr;

		RenderableProxy* proxy = bs_new<RenderableProxy>();

		for (UINT32 i = 0; i < mMeshData.mesh->getNumSubMeshes(); i++)
		{
			RenderableSubProxy* subProxy = bs_new<RenderableSubProxy>();
			subProxy->layer = mLayer;
			subProxy->worldTransform = SO()->getWorldTfrm();
			subProxy->mesh = &mMeshData.mesh->_getRenderData(i);

			HMaterial material;
			if (i < mMaterialData.size())
				material = mMaterialData[i].material;
			else
				material = mMaterialData[0].material;

			if (material == nullptr || !material.isLoaded())
				material = BuiltinMaterialManager::instance().createDummyMaterial();

			subProxy->material = material->_createProxy(allocator);

			proxy->subProxies.push_back(subProxy);
		}

		return proxy;
	}

	RTTITypeBase* Renderable::getRTTIStatic()
	{
		return RenderableRTTI::instance();
	}

	RTTITypeBase* Renderable::getRTTI() const
	{
		return Renderable::getRTTIStatic();
	}
}