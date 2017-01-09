//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsRendererObject.h"

namespace bs { namespace ct
{
	PerObjectParamDef gPerObjectParamDef;
	PerCallParamDef gPerCallParamDef;

	RendererObject::RendererObject()
	{
		perObjectParamBuffer = gPerObjectParamDef.createBuffer();
		perCallParamBuffer = gPerCallParamDef.createBuffer();
	}

	void RendererObject::updatePerObjectBuffer()
	{
		Matrix4 worldTransform = renderable->getTransform();
		Matrix4 worldNoScaleTransform = renderable->getTransformNoScale();

		gPerObjectParamDef.gMatWorld.set(perObjectParamBuffer, worldTransform);
		gPerObjectParamDef.gMatInvWorld.set(perObjectParamBuffer, worldTransform.inverseAffine());
		gPerObjectParamDef.gMatWorldNoScale.set(perObjectParamBuffer, worldNoScaleTransform);
		gPerObjectParamDef.gMatInvWorldNoScale.set(perObjectParamBuffer, worldNoScaleTransform.inverseAffine());
		gPerObjectParamDef.gWorldDeterminantSign.set(perObjectParamBuffer, worldTransform.determinant3x3() >= 0.0f ? 1.0f : -1.0f);
	}

	void RendererObject::updatePerCallBuffer(const Matrix4& viewProj, bool flush)
	{
		Matrix4 worldViewProjMatrix = viewProj * renderable->getTransform();

		gPerCallParamDef.gMatWorldViewProj.set(perCallParamBuffer, worldViewProjMatrix);

		if(flush)
			perCallParamBuffer->flushToGPU();
	}
}}