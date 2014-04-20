#include "BsRenderable.h"
#include "BsRenderableRTTI.h"
#include "CmSceneObject.h"
#include "CmMesh.h"
#include "CmMaterial.h"
#include "CmRenderQueue.h"

namespace BansheeEngine
{
	Renderable::Renderable(const HSceneObject& parent)
		:Component(parent), mLayer(1)
	{
		setName("Renderable");

		mMaterials.resize(1);
		mMatViewProjParam.resize(1);
	}

	void Renderable::setNumMaterials(UINT32 numMaterials)
	{
		mMaterials.resize(numMaterials);
		mMatViewProjParam.resize(numMaterials);
	}

	void Renderable::setMaterial(UINT32 idx, HMaterial material)
	{
		mMaterials[idx] = material;
		mMatViewProjParam[idx] = material->getParamMat4("matViewProjection");
	}

	void Renderable::render(RenderQueue& renderQueue, const Matrix4& viewProjMatrix)
	{
		if(mMesh == nullptr || !mMesh.isLoaded())
			return;

		bool hasAtLeastOneMaterial = false;
		UINT32 idx = 0;
		for(auto& material : mMaterials)
		{
			if(material != nullptr)
			{
				hasAtLeastOneMaterial = true;

				if(!material.isLoaded()) // We wait until all materials are loaded
					return;
			}

			// TODO - Do different things depending on material and renderable settings

			// TODO - Renderer should ensure shader is compatible with it, and it contains all the needed parameters
			// (probably at an earlier stage). e.g. I want the user to be warned if the shader doesn't contain matViewProjection param
			// (or should we just ignore such missing parameters?)

			mMatViewProjParam[idx].set(viewProjMatrix);
			idx++;
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

				renderQueue.add(mat.getInternalPtr(), mMesh.getInternalPtr(), i, mWorldBounds[i].getCenter());
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