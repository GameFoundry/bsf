#include "BsRenderable.h"
#include "BsRenderableRTTI.h"
#include "CmSceneObject.h"
#include "CmMesh.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	Renderable::Renderable(const HSceneObject& parent)
		:Component(parent), mLayer(1)
	{
		mMaterials.resize(1);
	}

	void Renderable::setNumMaterials(CM::UINT32 numMaterials)
	{
		mMaterials.resize(numMaterials);
	}

	void Renderable::setMaterial(CM::UINT32 idx, CM::HMaterial material)
	{
		mMaterials[idx] = material;
	}

	UINT32 Renderable::getNumRenderOperations() const
	{
		if(mMesh == nullptr || !mMesh.isLoaded())
			return 0;

		bool hasAtLeastOneMaterial = false;
		for(auto& material : mMaterials)
		{
			if(material != nullptr)
			{
				hasAtLeastOneMaterial = true;

				if(!material.isLoaded()) // We wait until all materials are loaded
					return 0;
			}
		}

		if(hasAtLeastOneMaterial)
			return mMesh->getNumSubMeshes();
		else
			return 0;
	}

	void Renderable::updateWorldBounds()
	{
		if(mMesh == nullptr || !mMesh.isLoaded())
			return;

		// TODO - This will likely need to be optimized in a more data friendly way
		// (e.g. store all bounds in a single array, and same with world matrices)
		mWorldBounds.resize(mMesh->getNumSubMeshes());
		for(UINT32 i = 0; i < (UINT32)mWorldBounds.size(); i++)
		{
			mWorldBounds[i] = mMesh->getBounds(i);
			mWorldBounds[i].transformAffine(SO()->getWorldTfrm());
		}
	}

	RenderOperation Renderable::getRenderOperation(CM::UINT32 idx) const
	{
		// TODO - Creating a RenderOperation each call might be excessive considering this will be called a few thousand times a frame
		RenderOperation ro;
		
		if(idx < mMaterials.size() && mMaterials[idx] != nullptr)
		{
			ro.material = mMaterials[idx];
		}
		else
		{
			for(auto& iter = mMaterials.rbegin(); iter != mMaterials.rend(); ++iter)
			{
				if((*iter) != nullptr)
				{
					ro.material = *iter;
					break;
				}
			}
		}

		ro.meshData = mMesh->getSubMeshData(idx);
		ro.worldPosition = mWorldBounds[idx].getCenter();

		return ro;
	}

	void Renderable::setLayer(UINT64 layer)
	{
		bool isPow2 = layer && !( (layer-1) & layer);

		if(!isPow2)
			CM_EXCEPT(InvalidParametersException, "Invalid layer provided. Only one layer bit may be set.");

		mLayer = layer;
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