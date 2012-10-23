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
#include "CmException.h"

namespace CamelotEngine
{
    //---------------------------------------------------------------------------
    HighLevelGpuProgram::HighLevelGpuProgram()
        : GpuProgram(), 
        mHighLevelLoaded(false), mAssemblerProgram(0), mConstantDefsBuilt(false)
    {
    }
    //---------------------------------------------------------------------------
    void HighLevelGpuProgram::load()
    {
		if (isSupported())
		{
			// load self 
			loadHighLevel();

			// create low-level implementation
			createLowLevelImpl();

			// load constructed assembler program (if it exists)
			if (mAssemblerProgram != nullptr && mAssemblerProgram.get() != this)
			{
				mAssemblerProgram->load();
			}
		}
    }
    //---------------------------------------------------------------------------
    void HighLevelGpuProgram::unload()
    {   
        if (mAssemblerProgram != nullptr && mAssemblerProgram.get() != this)
        {
            mAssemblerProgram = nullptr;
        }

        unloadHighLevel();
		resetCompileError();
    }
    //---------------------------------------------------------------------------
    HighLevelGpuProgram::~HighLevelGpuProgram()
    {
        // superclasses will trigger unload
    }
    //---------------------------------------------------------------------------
    GpuProgramParametersSharedPtr HighLevelGpuProgram::createParameters(void)
    {
		// Lock mutex before allowing this since this is a top-level method
		// called outside of the load()
		CM_LOCK_AUTO_MUTEX

        // Make sure param defs are loaded
        GpuProgramParametersSharedPtr params = GpuProgramParametersSharedPtr(new GpuProgramParameters());
		// Only populate named parameters if we can support this program
		if (this->isSupported())
		{
			populateParameterNames(params);
		}

		// Copy in default parameters if present
		if (mDefaultParams != nullptr)
			params->copyConstantsFrom(*(mDefaultParams.get()));
        return params;
    }
    //---------------------------------------------------------------------------
    void HighLevelGpuProgram::loadHighLevel(void)
    {
        if (!mHighLevelLoaded)
        {
			try 
			{
				loadHighLevelImpl();
				mHighLevelLoaded = true;

				assert(mDefaultParams == nullptr); // TODO - These two lines and replicated both here and in GpuProgram. I should probably add another load method that holds it all in one place?
				mDefaultParams = createParameters();

			}
			catch (const Exception& e)
			{
				//// will already have been logged
				//LogManager::getSingleton().stream()
				//	<< "High-level program " << mName << " encountered an error "
				//	<< "during loading and is thus not supported.\n"
				//	<< e.getFullDescription();

				mCompileError = true;
				throw;
			}
        }
    }
    //---------------------------------------------------------------------------
    void HighLevelGpuProgram::unloadHighLevel(void)
    {
        if (mHighLevelLoaded)
        {
            unloadHighLevelImpl();
			// Clear saved constant defs
			mConstantDefsBuilt = false;
			createParameterMappingStructures(true);

            mHighLevelLoaded = false;
        }
    }
    //---------------------------------------------------------------------------
    void HighLevelGpuProgram::loadHighLevelImpl(void)
    {
        loadFromSource();


    }
	//---------------------------------------------------------------------
	const GpuNamedConstants& HighLevelGpuProgram::getConstantDefinitions() const
	{
		if (!mConstantDefsBuilt)
		{
			buildConstantDefinitions();
			mConstantDefsBuilt = true;
		}
		return *mConstantDefs.get();

	}
	//---------------------------------------------------------------------
	void HighLevelGpuProgram::populateParameterNames(GpuProgramParametersSharedPtr params)
	{
		getConstantDefinitions();
		params->_setNamedConstants(mConstantDefs);
		// also set logical / physical maps for programs which use this
		params->_setLogicalIndexes(mFloatLogicalToPhysical, mIntLogicalToPhysical, mSamplerLogicalToPhysical);
	}
}
