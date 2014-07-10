//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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
		:Component(parent), mLayer(1), mCoreDirtyFlags(0xFFFFFFFF), mActiveProxy(nullptr)
	{
		setName("Renderable");

		mMaterialData.resize(1);
	}

	void Renderable::setMesh(HMesh mesh)
	{
		mMeshData = mesh;
		mMaterialData.resize(mesh->getNumSubMeshes());

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

	HMaterial Renderable::getMaterial(UINT32 idx) const
	{ 
		if (mMaterialData[idx].material != nullptr)
			return mMaterialData[idx].material;
		else
			return mMaterialData[0].material;
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
			if (materialData.material != nullptr && materialData.material.isLoaded() && materialData.material->_isCoreDirty(MaterialDirtyFlag::Material))
				return true;
		}

		if (mMeshData.mesh != nullptr && mMeshData.mesh.isLoaded() && mMeshData.mesh->_isCoreDirty(MeshDirtyFlag::Mesh))
			return true;

		return mCoreDirtyFlags != 0; 
	}

	void Renderable::_markCoreClean()
	{
		for (auto& materialData : mMaterialData)
		{
			if (materialData.material != nullptr && materialData.material.isLoaded())
				materialData.material->_markCoreClean(MaterialDirtyFlag::Material);
		}

		if (mMeshData.mesh != nullptr && mMeshData.mesh.isLoaded())
			mMeshData.mesh->_markCoreClean(MeshDirtyFlag::Mesh);

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

		bool markMeshProxyClean = false;
		for (UINT32 i = 0; i < mMeshData.mesh->getNumSubMeshes(); i++)
		{
			RenderableElement* renElement = bs_new<RenderableElement>();
			renElement->layer = mLayer;
			renElement->worldTransform = SO()->getWorldTfrm();

			if (mMeshData.mesh->_isCoreDirty(MeshDirtyFlag::Proxy))
			{
				mMeshData.mesh->_setActiveProxy(i, mMeshData.mesh->_createProxy(i));
				markMeshProxyClean = true;
			}

			renElement->mesh = mMeshData.mesh->_getActiveProxy(i);

			HMaterial material;
			if (mMaterialData[i].material != nullptr)
				material = mMaterialData[i].material;
			else
				material = mMaterialData[0].material;

			if (material == nullptr || !material.isLoaded())
				material = BuiltinMaterialManager::instance().createDummyMaterial();

			if (material->_isCoreDirty(MaterialDirtyFlag::Proxy))
			{
				material->_setActiveProxy(material->_createProxy());
				material->_markCoreClean(MaterialDirtyFlag::Proxy);
			}

			renElement->material = material->_getActiveProxy();

			proxy->renderableElements.push_back(renElement);
		}

		if (markMeshProxyClean)
		{
			mMeshData.mesh->_markCoreClean(MeshDirtyFlag::Proxy);
		}

		proxy->renderableType = RenType_LitTextured;

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