//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsRenderableHandler.h"
#include "BsGpuParams.h"
#include "BsMaterial.h"
#include "BsRenderableElement.h"

namespace BansheeEngine
{
	void RenderableHandler::bindGlobalBuffers(const RenderableElement& element)
	{
		for (auto& rendererBuffer : element.rendererBuffers)
		{
			SPtr<PassParametersCore> passParams = element.material->getPassParameters(rendererBuffer.passIdx);
			passParams->getParamByIdx(rendererBuffer.paramsIdx)->setParamBlockBuffer(rendererBuffer.slotIdx, rendererBuffer.buffer);
		}
	}
}