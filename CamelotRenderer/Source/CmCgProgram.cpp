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
#include "CmCgProgram.h"
#include "CmGpuProgramManager.h"
#include "CmHighLevelGpuProgramManager.h"
#include "CmRenderSystem.h"
#include "CmDebug.h"
#include "CmException.h"
#include "CmCgProgramRTTI.h"

namespace CamelotEngine {

	void checkForCgError(const String& ogreMethod, const String& errorTextPrefix, CGcontext context)
	{
		CGerror error = cgGetError();
		if (error != CG_NO_ERROR)
		{
			String msg = errorTextPrefix + cgGetErrorString(error); 

			if (error == CG_COMPILER_ERROR)
			{
				// Get listing with full compile errors
				msg = msg + "\n" + cgGetLastListing(context);
			}

			CM_EXCEPT(InternalErrorException, msg);
		}
	}
    //-----------------------------------------------------------------------
    void CgProgram::selectProfile(void)
    {
        mSelectedProfile.clear();
        mSelectedCgProfile = CG_PROFILE_UNKNOWN;

		RenderSystem* rs = RenderSystem::instancePtr();
		const String& rsName = rs->getName();

		mSelectedProfile = GpuProgramManager::instance().gpuProgProfileToRSSpecificProfile(mProfile);
		GpuProgramManager& gpuMgr = GpuProgramManager::instance();
		if (gpuMgr.isSyntaxSupported(mSelectedProfile))
		{
			mSelectedCgProfile = cgGetProfile(mSelectedProfile.c_str());
			// Check for errors
			checkForCgError("CgProgram::selectProfile", 
				"Unable to find CG profile enum for program.", mCgContext);
		}
		else
			mSelectedProfile.clear();
    }
    //-----------------------------------------------------------------------
    void CgProgram::loadFromSource(void)
    {
        // Create Cg Program
        selectProfile();
		if (mSelectedCgProfile == CG_PROFILE_UNKNOWN)
		{
			LOGWRN("Attempted to load Cg program but no supported profile was found.");
			return;
		}

		// TODO PORT - This doesn't load includes
		// deal with includes
		String sourceToUse = mSource;
		//String sourceToUse = resolveCgIncludes(mSource, this, mFilename);
        mCgProgram = cgCreateProgram(mCgContext, CG_SOURCE, sourceToUse.c_str(), 
            mSelectedCgProfile, mEntryPoint.c_str(), nullptr);

        // Check for errors
        checkForCgError("CgProgram::loadFromSource", 
            "Unable to compile Cg program", mCgContext);

		// ignore any previous error
		if (mSelectedCgProfile != CG_PROFILE_UNKNOWN)
		{
			String sourceFromCg = cgGetProgramString(mCgProgram, CG_COMPILED_PROGRAM);

			// Create a low-level program, give it the same name as us
			mAssemblerProgram = 
				HighLevelGpuProgramManager::instance().create(
				sourceFromCg, mEntryPoint, RenderSystem::instance().getShadingLanguageName(), mType, mProfile, nullptr);

			// Shader params need to be forwarded to low level implementation
			mAssemblerProgram->setAdjacencyInfoRequired(isAdjacencyInfoRequired());
		}

		cgDestroyProgram(mCgProgram);
		checkForCgError("CgProgram::unloadImpl", "Error while unloading Cg program", mCgContext);
		mCgProgram = nullptr;
    }

    void CgProgram::unload_internal(void)
    {
		HighLevelGpuProgram::destroy_internal();
    }

	GpuParamsPtr CgProgram::createParameters()
	{
		return getWrappedProgram()->createParameters();
	}

	GpuProgramPtr CgProgram::getBindingDelegate(void)
	{
		if(mAssemblerProgram)
			return getWrappedProgram()->getBindingDelegate();

		return nullptr;
	}

	HighLevelGpuProgramPtr CgProgram::getWrappedProgram() const
	{
		HighLevelGpuProgramPtr wrappedProgram = std::static_pointer_cast<HighLevelGpuProgram>(mAssemblerProgram);

		return wrappedProgram;
	}

	CgProgram::CgProgram(CGcontext context, const String& source, const String& entryPoint, const String& language, 
		GpuProgramType gptype, GpuProgramProfile profile, const vector<GpuProgIncludePtr>::type* includes, bool isAdjacencyInfoRequired)
        : HighLevelGpuProgram(source, entryPoint, language, gptype, profile, includes, isAdjacencyInfoRequired), 
        mCgContext(context), mCgProgram(0), 
        mSelectedCgProfile(CG_PROFILE_UNKNOWN)
    {

    }

    CgProgram::~CgProgram()
    {
        unload_internal();
    }

    bool CgProgram::isSupported(void) const
    {
        if (!isRequiredCapabilitiesSupported())
            return false;

		String selectedProfile = GpuProgramManager::instance().gpuProgProfileToRSSpecificProfile(mProfile);
		if (GpuProgramManager::instance().isSyntaxSupported(selectedProfile))
			return true;

        return false;
    }

    const String& CgProgram::getLanguage(void) const
    {
        static const String language = "cg";

        return language;
    }

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	RTTITypeBase* CgProgram::getRTTIStatic()
	{
		return CgProgramRTTI::instance();
	}

	RTTITypeBase* CgProgram::getRTTI() const
	{
		return CgProgram::getRTTIStatic();
	}
}
