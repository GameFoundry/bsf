#include "BsRenderableController.h"
#include "BsGpuParams.h"
#include "BsMaterial.h"
#include "BsRenderableElement.h"

namespace BansheeEngine
{
	void RenderableController::bindGlobalBuffers(const RenderableElement& element)
	{
		for (auto& rendererBuffer : element.rendererBuffers)
		{
			SPtr<PassParametersCore> passParams = element.material->getPassParameters(rendererBuffer.passIdx);
			passParams->getParamByIdx(rendererBuffer.paramsIdx)->setParamBlockBuffer(rendererBuffer.slotIdx, rendererBuffer.buffer);
		}
	}
}