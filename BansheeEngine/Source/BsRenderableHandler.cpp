//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsRenderableHandler.h"
#include "BsRenderableProxy.h"
#include "BsGpuParams.h"

namespace BansheeEngine
{
	void RenderableHandler::bindGlobalBuffers(const RenderableElement* element)
	{
		for (auto& rendererBuffer : element->material->rendererBuffers)
		{
			element->material->params[rendererBuffer.paramsIdx]->setParamBlockBuffer(rendererBuffer.slotIdx, rendererBuffer.buffer);
		}
	}
}