//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsRendererObject.h"

namespace bs
{
	void RendererObject::updatePerObjectBuffer()
	{
		Matrix4 worldTransform = renderable->getTransform();
		Matrix4 worldNoScaleTransform = renderable->getTransformNoScale();

		perObjectParams.gMatWorld.set(worldTransform);
		perObjectParams.gMatInvWorld.set(worldTransform.inverseAffine());
		perObjectParams.gMatWorldNoScale.set(worldNoScaleTransform);
		perObjectParams.gMatInvWorldNoScale.set(worldNoScaleTransform.inverseAffine());
		perObjectParams.gWorldDeterminantSign.set(worldTransform.determinant3x3() >= 0.0f ? 1.0f : -1.0f);
	}

	void RendererObject::updatePerCallBuffer(const Matrix4& viewProj, bool flush)
	{
		Matrix4 worldViewProjMatrix = viewProj * renderable->getTransform();

		perCallParams.gMatWorldViewProj.set(worldViewProjMatrix);

		if(flush)
			perCallParams.flushToGPU();
	}
}