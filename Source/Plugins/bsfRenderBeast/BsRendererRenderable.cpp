//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsRendererRenderable.h"

namespace bs { namespace ct
{
	PerObjectParamDef gPerObjectParamDef;
	PerCallParamDef gPerCallParamDef;

	RendererRenderable::RendererRenderable()
	{
		perObjectParamBuffer = gPerObjectParamDef.createBuffer();
		perCallParamBuffer = gPerCallParamDef.createBuffer();
	}

	void RendererRenderable::updatePerObjectBuffer()
	{
		Matrix4 worldTransform = renderable->getMatrix();
		Matrix4 worldNoScaleTransform = renderable->getMatrixNoScale();

		gPerObjectParamDef.gMatWorld.set(perObjectParamBuffer, worldTransform);
		gPerObjectParamDef.gMatInvWorld.set(perObjectParamBuffer, worldTransform.inverseAffine());
		gPerObjectParamDef.gMatWorldNoScale.set(perObjectParamBuffer, worldNoScaleTransform);
		gPerObjectParamDef.gMatInvWorldNoScale.set(perObjectParamBuffer, worldNoScaleTransform.inverseAffine());
		gPerObjectParamDef.gWorldDeterminantSign.set(perObjectParamBuffer, worldTransform.determinant3x3() >= 0.0f ? 1.0f : -1.0f);
	}

	void RendererRenderable::updatePerCallBuffer(const Matrix4& viewProj, bool flush)
	{
		Matrix4 worldViewProjMatrix = viewProj * renderable->getMatrix();

		gPerCallParamDef.gMatWorldViewProj.set(perCallParamBuffer, worldViewProjMatrix);

		if(flush)
			perCallParamBuffer->flushToGPU();
	}
}}