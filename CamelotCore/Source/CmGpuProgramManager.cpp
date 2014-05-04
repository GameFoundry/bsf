/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "CmGpuProgramManager.h"
#include "CmRenderSystem.h"


namespace BansheeEngine {
	//---------------------------------------------------------------------------
	GpuProgramManager::GpuProgramManager()
	{
		// subclasses should register with resource group manager
	}
	//---------------------------------------------------------------------------
	GpuProgramManager::~GpuProgramManager()
	{
		// subclasses should unregister with resource group manager
	}
    //---------------------------------------------------------------------------
	GpuProgramPtr GpuProgramManager::createProgram(const String& source, const String& entryPoint, GpuProgramType gptype, GpuProgramProfile profile)
    {
		GpuProgramPtr prg = create(source, entryPoint, gptype, profile);
		prg->_setThisPtr(prg);

		// TODO: Gpu programs get initialized by their parent HighLevelGpuProgram. I might handle that more intuitively later but
		// it works just fine as it is
		//prg->initialize();

        return prg;
    }
    //---------------------------------------------------------------------------
	const GpuProgramManager::SyntaxCodes& GpuProgramManager::getSupportedSyntax(void) const
    {
		// Use the current render system
		RenderSystem* rs = BansheeEngine::RenderSystem::instancePtr();

		// Get the supported syntaxed from RenderSystemCapabilities 
		return rs->getCapabilities()->getSupportedShaderProfiles();
    }
    //---------------------------------------------------------------------------
    bool GpuProgramManager::isSyntaxSupported(const String& syntaxCode) const
        {
			// Use the current render system
			RenderSystem* rs = BansheeEngine::RenderSystem::instancePtr();

			// Get the supported syntaxed from RenderSystemCapabilities 
			return rs->getCapabilities()->isShaderProfileSupported(syntaxCode);
		}

	//---------------------------------------------------------------------------
	String GpuProgramManager::gpuProgProfileToRSSpecificProfile(GpuProgramProfile gpuProgProfile) const
	{
		// Use the current render system
		RenderSystem* rs = BansheeEngine::RenderSystem::instancePtr();

		return  rs->getCapabilities()->gpuProgProfileToRSSpecificProfile(gpuProgProfile);
	}
}
