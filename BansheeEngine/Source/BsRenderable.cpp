#include "BsRenderable.h"
#include "BsRenderableRTTI.h"
#include "CmSceneObject.h"
#include "CmMesh.h"
#include "CmRenderQueue.h"

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

	void Renderable::render(CM::RenderQueue& renderQueue)
	{
		if(mMesh == nullptr || !mMesh.isLoaded())
			return;

		bool hasAtLeastOneMaterial = false;
		for(auto& material : mMaterials)
		{
			if(material != nullptr)
			{
				hasAtLeastOneMaterial = true;

				if(!material.isLoaded()) // We wait until all materials are loaded
					return;
			}
		}

		if(hasAtLeastOneMaterial)
		{
			for(UINT32 i = 0; i < mMesh->getNumSubMeshes(); i++)
			{
				HMaterial mat;
				if(i < mMaterials.size() && mMaterials[i] != nullptr)
				{
					mat = mMaterials[i];
				}
				else
				{
					for(auto& iter = mMaterials.rbegin(); iter != mMaterials.rend(); ++iter)
					{
						if((*iter) != nullptr)
						{
							mat = *iter;
							break;
						}
					}
				}

				renderQueue.add(mat, mMesh, i, mWorldBounds[i].getCenter());
			}

		}
		else
			return;
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