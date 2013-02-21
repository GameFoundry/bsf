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
#ifndef __CgProgram_H__
#define __CgProgram_H__

#include "CmPrerequisites.h"
#include <Cg/cg.h>
#include "CmHighLevelGpuProgram.h"

namespace CamelotEngine {
    /** Specialisation of HighLevelGpuProgram to provide support for nVidia's CG language.
    @remarks
        In its current state Cg only acts as a translator and internally creates either a HLSL
		or a GLSL object. Cg doesn't support SM5 for GLSL (only Nvidia cards are supported,
		which makes it very much useless for majority of projects).
    */
    class CM_EXPORT CgProgram : public HighLevelGpuProgram
    {
    public:
        ~CgProgram();

        /** Gets the entry point defined for this program. */
        const String& getEntryPoint(void) const { return mEntryPoint; }
        /// Overridden from GpuProgram
        bool isSupported(void) const;
        /// Overridden from GpuProgram
        const String& getLanguage(void) const;

		/**
		 * @copydoc HighLevelGpuProgram::getBindingDelegate().
		 */
		GpuProgramPtr getBindingDelegate();

		/**
		 * @copydoc HighLevelGpuProgram::createParameters().
		 */
		GpuParamsPtr createParameters();

    protected:
		friend class CgProgramFactory;

        /// The CG context to use, passed in by factory
        CGcontext mCgContext;
        /// Program handle
        CGprogram mCgProgram;
		String mSelectedProfile;
		CGprofile mSelectedCgProfile;

		CgProgram(CGcontext context, const String& source, const String& entryPoint, const String& language, 
			GpuProgramType gptype, GpuProgramProfile profile, const vector<GpuProgIncludeHandle>::type* includes, 
			bool isAdjacencyInfoRequired = false);

        /** Internal load implementation, must be implemented by subclasses.
        */
        void loadFromSource(void);
		/**
		 * @copydoc GpuProgram::unload_internal()
		 */
        void unload_internal(void);

		/**
		 * @brief	Cg acts only like a wrapper to a rendersystem specific program.
		 * 			This returns the actual program we are wrapping.
		 */
		HighLevelGpuProgramPtr getWrappedProgram() const;

        /// Internal method which works out which profile to use for this program
        void selectProfile(void);

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class CgProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
    };

	/** Utility function, checks Cg for errors, throw exception if detected.
    @param ogreMethod Ogre method name, as Class::method
    @param errorTextPrefix The text to prefix the Cg error text with
    */
    void checkForCgError(const String& ogreMethod, const String& errorTextPrefix, CGcontext context);
}

#endif
