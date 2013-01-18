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
#ifndef __D3D9HLSLProgram_H__
#define __D3D9HLSLProgram_H__

#include "CmD3D9Prerequisites.h"
#include "CmHighLevelGpuProgram.h"

namespace CamelotEngine {
    /** Specialisation of HighLevelGpuProgram to provide support for D3D9 
        High-Level Shader Language (HLSL).
    */
    class CM_D3D9_EXPORT D3D9HLSLProgram : public HighLevelGpuProgram
    {
	public:
		LPD3DXBUFFER getMicroCode();
	public:
		/// Shader optimisation level
		enum OptimisationLevel
		{
			/// default - no optimisation in debug mode, OPT_1 in release mode
			OPT_DEFAULT,
			/// No optimisation
			OPT_NONE,
			/// Optimisation level 0
			OPT_0, 
			/// Optimisation level 1
			OPT_1,
			/// Optimisation level 2
			OPT_2, 
			/// Optimisation level 3
			OPT_3
		};
	protected:
		OptimisationLevel mOptimisationLevel;

    public:
        ~D3D9HLSLProgram();

        /** Sets the preprocessor defines use to compile the program. */
        void setPreprocessorDefines(const String& defines) { mPreprocessorDefines = defines; }
        /** Sets the preprocessor defines use to compile the program. */
        const String& getPreprocessorDefines(void) const { return mPreprocessorDefines; }
        /** Sets whether matrix packing in column-major order. */ 
        void setColumnMajorMatrices(bool columnMajor) { mColumnMajorMatrices = columnMajor; }
        /** Gets whether matrix packed in column-major order. */
        bool getColumnMajorMatrices(void) const { return mColumnMajorMatrices; }
		/** Sets the optimisation level to use.
		@param opt Optimisation level
		*/
		void setOptimisationLevel(OptimisationLevel opt) { mOptimisationLevel = opt; }

		/** Gets the optimisation level to use. */
		OptimisationLevel getOptimisationLevel() const { return mOptimisationLevel; }

        /// Overridden from GpuProgram
        bool isSupported(void) const;
        /// Overridden from GpuProgram
        GpuParamsPtr createParameters();
        /// Overridden from GpuProgram
        const String& getLanguage(void) const;

    protected:
		friend class D3D9HLSLProgramFactory;

		D3D9HLSLProgram(const String& source, const String& entryPoint, const String& language, 
			GpuProgramType gptype, GpuProgramProfile profile, bool isAdjacencyInfoRequired = false);

        /** Internal load implementation, must be implemented by subclasses.
        */
        void loadFromSource(void);

		/**
		 * @copydoc HighLevelGpuProgram::destroy_internal.
		 */
        void destroy_internal();

        String mPreprocessorDefines;
        bool mColumnMajorMatrices;

        LPD3DXBUFFER mpMicroCode;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class D3D9HLSLProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
    };
}

#endif
