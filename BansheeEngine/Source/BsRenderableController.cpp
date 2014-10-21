#include "BsRenderableController.h"
#include "BsRenderableProxy.h"
#include "BsGpuParams.h"

namespace BansheeEngine
{
	void RenderableController::bindGlobalBuffers(const RenderableElement* element)
	{
		for (auto& rendererBuffer : element->material->rendererBuffers)
		{
			element->material->params[rendererBuffer.paramsIdx]->setParamBlockBuffer(rendererBuffer.slotIdx, rendererBuffer.buffer);
		}
	}
}