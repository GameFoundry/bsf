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
#include "CmHighLevelGpuProgram.h"
#include "CmHighLevelGpuProgramManager.h"
#include "CmRenderSystem.h"
#include "CmException.h"
#include "CmRenderSystem.h"
#include "CmAsyncOp.h"

#if CM_DEBUG_MODE
#define THROW_IF_NOT_RENDER_THREAD throwIfNotRenderThread();
#else
#define THROW_IF_NOT_RENDER_THREAD 
#endif

namespace CamelotEngine
{
    //---------------------------------------------------------------------------
	HighLevelGpuProgram::HighLevelGpuProgram(const String& source, const String& entryPoint, const String& language, 
		GpuProgramType gptype, GpuProgramProfile profile, bool isAdjacencyInfoRequired)
        : GpuProgram(source, entryPoint, language, gptype, profile, isAdjacencyInfoRequired), 
        mAssemblerProgram(0)
    {
    }
	//---------------------------------------------------------------------------
	void HighLevelGpuProgram::initialize()
	{
		RenderSystem::instancePtr()->queueCommand(boost::bind(&HighLevelGpuProgram::initialize_internal, this));
	}
    //---------------------------------------------------------------------------
    void HighLevelGpuProgram::initialize_internal()
    {
		if (isSupported())
		{
			// load self 
			loadFromSource();

			// load constructed assembler program (if it exists)
			if (mAssemblerProgram != nullptr && mAssemblerProgram.get() != this)
			{
				mAssemblerProgram->initialize_internal();
			}
		}

		Resource::initialize_internal();
    }
    //---------------------------------------------------------------------------
    void HighLevelGpuProgram::unload_internal()
    {   
        if (mAssemblerProgram != nullptr && mAssemblerProgram.get() != this)
        {
            mAssemblerProgram = nullptr;
        }

		resetCompileError();

		GpuProgram::unload_internal();
    }
    //---------------------------------------------------------------------------
    HighLevelGpuProgram::~HighLevelGpuProgram()
    {
        // superclasses will trigger unload
    }
	//---------------------------------------------------------------------
	HighLevelGpuProgramPtr HighLevelGpuProgram::create(const String& source, const String& entryPoint, 
		const String& language, GpuProgramType gptype, GpuProgramProfile profile)
	{
		return HighLevelGpuProgramManager::instance().create(source, entryPoint, language, gptype, profile);
	}
}

#undef THROW_IF_NOT_RENDER_THREAD 