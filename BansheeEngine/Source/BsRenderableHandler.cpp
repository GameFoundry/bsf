#include "BsRenderableHandler.h"
#include "BsRenderableHandlerRTTI.h"
#include "BsSceneObject.h"
#include "BsBuiltinResources.h"
#include "BsMesh.h"
#include "BsMaterial.h"
#include "BsRenderQueue.h"
#include "BsBounds.h"

namespace BansheeEngine
{
	RenderableHandler::MeshData::MeshData(const HMesh& mesh)
	{
		this->mesh = mesh;
		isLoaded = mesh.isLoaded();
	}

	RenderableHandler::MaterialData::MaterialData(const HMaterial& material)
	{
		this->material = material;
		isLoaded = material.isLoaded();
	}

	RenderableHandler::RenderableHandler()
		:mLayer(1), mCoreDirtyFlags(0xFFFFFFFF), mActiveProxy(nullptr)
	{
		mMaterialData.resize(1);
	}

	void RenderableHandler::setMesh(HMesh mesh)
	{
		mMeshData = mesh;
		mMaterialData.resize(mesh->getNumSubMeshes());

		_markCoreDirty();
	}

	void RenderableHandler::setMaterial(UINT32 idx, HMaterial material)
	{
		mMaterialData[idx] = material;

		_markCoreDirty();
	}

	void RenderableHandler::setMaterial(HMaterial material)
	{
		setMaterial(0, material);
	}

	HMaterial RenderableHandler::getMaterial(UINT32 idx) const
	{
		if (mMaterialData[idx].material != nullptr)
			return mMaterialData[idx].material;
		else
			return mMaterialData[0].material;
	}

	void RenderableHandler::setLayer(UINT64 layer)
	{
		bool isPow2 = layer && !((layer - 1) & layer);

		if (!isPow2)
			BS_EXCEPT(InvalidParametersException, "Invalid layer provided. Only one layer bit may be set.");

		mLayer = layer;
		_markCoreDirty();
	}

	bool RenderableHandler::_isCoreDirty() const
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

	void RenderableHandler::_markCoreClean()
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

	void RenderableHandler::updateResourceLoadStates() const
	{
		if (!mMeshData.isLoaded && mMeshData.mesh != nullptr && mMeshData.mesh.isLoaded())
		{
			mMeshData.isLoaded = true;

			_markCoreDirty();
		}

		for (auto& materialData : mMaterialData)
		{
			if (!materialData.isLoaded && materialData.material != nullptr && materialData.material.isLoaded())
			{
				materialData.isLoaded = true;

				_markCoreDirty();
			}
		}
	}

	RenderableProxyPtr RenderableHandler::_createProxy(const Matrix4& worldTransform) const
	{
		if (mMeshData.mesh == nullptr || !mMeshData.mesh.isLoaded())
			return nullptr;

		RenderableProxyPtr proxy = bs_shared_ptr<RenderableProxy>();

		bool markMeshProxyClean = false;
		for (UINT32 i = 0; i < mMeshData.mesh->getNumSubMeshes(); i++)
		{
			RenderableElement* renElement = bs_new<RenderableElement>();
			renElement->layer = mLayer;
			renElement->worldTransform = worldTransform;

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
				material = BuiltinResources::instance().createDummyMaterial();

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

	RTTITypeBase* RenderableHandler::getRTTIStatic()
	{
		return RenderableHandlerRTTI::instance();
	}

	RTTITypeBase* RenderableHandler::getRTTI() const
	{
		return RenderableHandler::getRTTIStatic();
	}
}