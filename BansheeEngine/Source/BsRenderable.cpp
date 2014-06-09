#include "BsRenderable.h"
#include "BsRenderableRTTI.h"
#include "BsSceneObject.h"
#include "BsBuiltinMaterialManager.h"
#include "BsMeshRenderData.h"
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
		:Component(parent), mLayer(1), mCoreDirtyFlags(0xFFFFFFFF), mActiveProxy(nullptr)
	{
		setName("Renderable");

		mMaterialData.resize(1);
	}

	void Renderable::setMesh(HMesh mesh)
	{
		mMeshData = mesh;

		markCoreDirty();
	}

	void Renderable::setNumMaterials(UINT32 numMaterials)
	{
		numMaterials = std::max(1U, numMaterials);

		mMaterialData.resize(numMaterials);

		markCoreDirty();
	}

	void Renderable::setMaterial(UINT32 idx, HMaterial material)
	{
		mMaterialData[idx] = material;

		markCoreDirty();
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
		markCoreDirty();
	}

	bool Renderable::_isCoreDirty() const
	{ 
		updateResourceLoadStates();

		for (auto& materialData : mMaterialData)
		{
			if (materialData.material != nullptr && materialData.material.isLoaded() && materialData.material->_isCoreDirty())
				return true;
		}

		if (mMeshData.mesh != nullptr && mMeshData.mesh.isLoaded() && mMeshData.mesh->_isCoreDirty())
			return true;

		return mCoreDirtyFlags; 
	}

	void Renderable::_markCoreClean()
	{
		for (auto& materialData : mMaterialData)
		{
			if (materialData.material != nullptr && materialData.material.isLoaded())
				materialData.material->_markCoreClean();
		}

		if (mMeshData.mesh != nullptr && mMeshData.mesh.isLoaded())
			mMeshData.mesh->_markCoreClean();

		mCoreDirtyFlags = 0;
	}

	void Renderable::updateResourceLoadStates() const
	{
		if (!mMeshData.isLoaded && mMeshData.mesh != nullptr && mMeshData.mesh.isLoaded())
		{
			mMeshData.isLoaded = true;

			markCoreDirty();
		}

		for (auto& materialData : mMaterialData)
		{
			if (!materialData.isLoaded && materialData.material != nullptr && materialData.material.isLoaded())
			{
				materialData.isLoaded = true;

				markCoreDirty();
			}
		}
	}

	RenderableProxyPtr Renderable::_createProxy() const
	{
		if (mMeshData.mesh == nullptr || !mMeshData.mesh.isLoaded())
			return nullptr;

		RenderableProxyPtr proxy = bs_shared_ptr<RenderableProxy>();

		for (UINT32 i = 0; i < mMeshData.mesh->getNumSubMeshes(); i++)
		{
			RenderableElement* renElement = bs_new<RenderableElement>();
			renElement->layer = mLayer;
			renElement->worldTransform = SO()->getWorldTfrm();
			renElement->mesh = mMeshData.mesh->_createProxy(i);

			HMaterial material;
			if (i < mMaterialData.size())
				material = mMaterialData[i].material;
			else
				material = mMaterialData[0].material;

			if (material == nullptr || !material.isLoaded())
				material = BuiltinMaterialManager::instance().createDummyMaterial();

			renElement->material = material->_createProxy();

			proxy->renderableElements.push_back(renElement);
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