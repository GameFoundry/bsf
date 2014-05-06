#include "CmGpuProgramManager.h"
#include "CmRenderSystem.h"

namespace BansheeEngine 
{
	GpuProgramManager::GpuProgramManager()
	{ }

	GpuProgramManager::~GpuProgramManager()
	{ }

	GpuProgramPtr GpuProgramManager::createProgram(const String& source, const String& entryPoint, GpuProgramType gptype, GpuProgramProfile profile)
    {
		GpuProgramPtr prg = create(source, entryPoint, gptype, profile);
		prg->_setThisPtr(prg);

		// TODO: GPU programs get initialized by their parent HighLevelGpuProgram. I might handle that more intuitively later but
		// it works just fine as it is
		//prg->initialize();

        return prg;
    }

	String GpuProgramManager::gpuProgProfileToRSSpecificProfile(GpuProgramProfile gpuProgProfile) const
	{
		RenderSystem* rs = BansheeEngine::RenderSystem::instancePtr();

		return  rs->getCapabilities()->gpuProgProfileToRSSpecificProfile(gpuProgProfile);
	}
}
