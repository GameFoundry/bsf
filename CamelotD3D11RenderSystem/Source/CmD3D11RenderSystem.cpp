#include "CmD3D11RenderSystem.h"
#include "CmRenderSystem.h"
#include "CmException.h"

namespace CamelotEngine
{
	D3D11Device& D3D11RenderSystem::getPrimaryDevice() 
	{ 
		CM_EXCEPT(NotImplementedException, "Not implemented"); 
	}
}