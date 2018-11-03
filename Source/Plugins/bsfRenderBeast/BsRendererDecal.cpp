//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsRendererDecal.h"
#include "BsRendererRenderable.h"

namespace bs { namespace ct
{
	DecalParamDef gDecalParamDef;

	RendererDecal::RendererDecal()
	{
		decalParamBuffer = gDecalParamDef.createBuffer();
		perObjectParamBuffer = gPerObjectParamDef.createBuffer();
		perCallParamBuffer = gPerCallParamDef.createBuffer();
	}

	void RendererDecal::updatePerObjectBuffer()
	{
		// TODO
		//const Matrix4 worldTransform = renderable->getMatrix();
		//const Matrix4 worldNoScaleTransform = renderable->getMatrixNoScale();

		//PerObjectBuffer::update(perObjectParamBuffer, worldTransform, worldNoScaleTransform);
	}

	void RendererDecal::updatePerCallBuffer(const Matrix4& viewProj, bool flush) const
	{
		// TODO
		//const Matrix4 worldViewProjMatrix = viewProj * renderable->getMatrix();

		//gPerCallParamDef.gMatWorldViewProj.set(perCallParamBuffer, worldViewProjMatrix);

		if(flush)
			perCallParamBuffer->flushToGPU();
	}
}}
