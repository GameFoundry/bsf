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
#include "CmException.h"
#include "CmCoreThread.h"
#include "CmAsyncOp.h"
#include "CmResources.h"

namespace BansheeEngine
{
	HighLevelGpuProgram::HighLevelGpuProgram(const String& source, const String& entryPoint, const String& language, 
		GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>::type* includes, bool isAdjacencyInfoRequired)
        : GpuProgram(source, entryPoint, language, gptype, profile, includes, isAdjacencyInfoRequired), 
        mAssemblerProgram(nullptr)
    {
    }

	HighLevelGpuProgram::~HighLevelGpuProgram()
	{

	}

    void HighLevelGpuProgram::initialize_internal()
    {
		if (isSupported())
		{
			if (mAssemblerProgram != nullptr && mAssemblerProgram.get() != this)
			{
				mAssemblerProgram->initialize();
			}
		}

		GpuProgram::initialize_internal();
    }

    void HighLevelGpuProgram::destroy_internal()
    {   
        mAssemblerProgram = nullptr;

		GpuProgram::destroy_internal();
    }

	HHighLevelGpuProgram HighLevelGpuProgram::create(const String& source, const String& entryPoint, 
		const String& language, GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>::type* includes)
	{
		HighLevelGpuProgramPtr programPtr = _createPtr(source, entryPoint, language, gptype, profile, includes);

		return static_resource_cast<HighLevelGpuProgram>(gResources().createResourceHandle(programPtr));
	}

	HighLevelGpuProgramPtr HighLevelGpuProgram::_createPtr(const String& source, const String& entryPoint, 
		const String& language, GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>::type* includes)
	{
		return HighLevelGpuProgramManager::instance().create(source, entryPoint, language, gptype, profile, includes);
	}
}