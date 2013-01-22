/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

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
#ifndef __GpuProgram_H_
#define __GpuProgram_H_

// Precompiler options
#include "CmPrerequisites.h"
#include "CmRenderOperation.h"
#include "CmGpuProgramParams.h"
#include "CmResource.h"
#include "CmGpuParamDesc.h"

namespace CamelotEngine {

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Resources
	*  @{
	*/
	/** Enumerates the types of programs which can run on the GPU. */
	enum GpuProgramType
	{
		GPT_VERTEX_PROGRAM,
		GPT_FRAGMENT_PROGRAM,
		GPT_GEOMETRY_PROGRAM,
		GPT_DOMAIN_PROGRAM,
		GPT_HULL_PROGRAM,
		GPT_COMPUTE_PROGRAM
	};

	enum GpuProgramProfile
	{
		GPP_NONE,
		GPP_PS_1_1,
		GPP_PS_1_2,
		GPP_PS_1_3,
		GPP_PS_1_4,
		GPP_PS_2_0,
		GPP_PS_2_x,
		GPP_PS_2_a,
		GPP_PS_2_b,
		GPP_PS_3_0,
		GPP_PS_3_x,
		GPP_PS_4_0,
		GPP_PS_4_1,
		GPP_PS_5_0,
		GPP_VS_1_1,
		GPP_VS_2_0,
		GPP_VS_2_x,
		GPP_VS_2_a,
		GPP_VS_3_0,
		GPP_VS_4_0,
		GPP_VS_4_1,
		GPP_VS_5_0,
		GPP_GS_4_0,
		GPP_GS_4_1,
		GPP_GS_5_0,
		GPP_HS_5_0,
		GPP_DS_5_0,
		GPP_CS_5_0
	};

	/** Defines a program which runs on the GPU such as a vertex or fragment program. 
	@remarks
		This class defines the low-level program in assembler code, the sort used to
		directly assemble into machine instructions for the GPU to execute. By nature,
		this means that the assembler source is rendersystem specific, which is why this
		is an abstract class - real instances are created through the RenderSystem. 
		If you wish to use higher level shading languages like HLSL and Cg, you need to 
		use the HighLevelGpuProgram class instead.
	*/
	class CM_EXPORT GpuProgram : public Resource
	{
	public:
		virtual ~GpuProgram();

		/**
		 * @brief	Performs GpuProgram initialization. Only callable from the render thread.
		 */
		virtual void initialize_internal();

        /** Gets the syntax code for this program e.g. arbvp1, fp20, vs_1_1 etc */
        virtual const String& getSyntaxCode(void) const { return mSyntaxCode; }

        /** Gets the assembler source for this program. */
        virtual const String& getSource(void) const { return mSource; }
        /// Get the program type
        virtual GpuProgramType getType(void) const { return mType; }
		virtual GpuProgramProfile getProfile() const { return mProfile; }
		virtual const String& getEntryPoint() const { return mEntryPoint; }

        /** Returns the GpuProgram which should be bound to the pipeline.
        @remarks
            This method is simply to allow some subclasses of GpuProgram to delegate
            the program which is bound to the pipeline to a delegate, if required. */
        virtual GpuProgram* getBindingDelegate(void) { return this; }

        /** Returns whether this program can be supported on the current renderer and hardware. */
        virtual bool isSupported(void) const;

		/** Sets whether this geometry program requires adjacency information
			from the input primitives.
		*/
		virtual void setAdjacencyInfoRequired(bool r) { mNeedsAdjacencyInfo = r; }
		/** Returns whether this geometry program requires adjacency information 
			from the input primitives.
		*/
		virtual bool isAdjacencyInfoRequired(void) const { return mNeedsAdjacencyInfo; }

        /** Creates a new parameters object compatible with this program definition. 
        @remarks
            It is recommended that you use this method of creating parameters objects
            rather than going direct to GpuProgramManager, because this method will
            populate any implementation-specific extras (like named parameters) where
            they are appropriate.
        */
		virtual GpuParamsPtr createParameters();

		const GpuParamDesc& getParamDesc() const { return mParametersDesc; }

		/** Returns a string that specifies the language of the gpu programs as specified
        in a material script. ie: asm, cg, hlsl, glsl
        */
        virtual const String& getLanguage(void) const;

	protected:
		/// The type of the program
		GpuProgramType mType;
		/// Does this (geometry) program require adjacency information?
		bool mNeedsAdjacencyInfo;
		/// Name of the shader entry method
		String mEntryPoint;
		/// Shader profiler that we are targeting (e.g. vs_1_1, etc.). Make sure profile matches the type.
		GpuProgramProfile mProfile;
        /// The assembler source of the program (may be blank until file loaded)
        String mSource;
        /// Syntax code e.g. arbvp1, vs_2_0 etc
        String mSyntaxCode;

		/**
		 * @brief	Contains information about all parameters in a shader.
		 */
		GpuParamDesc mParametersDesc;

		/**
		 * @brief	Initializes the gpu program. This must be called right after the program is
		 * 			constructed. Called by GpuManager upon creation, so usually you don't want
		 * 			to call this manually.
		 *		 
		 * @note	Initialization is not done immediately, and is instead just scheduled on the
		 * 			render thread. Unless called from render thread, in which case it is initialized
		 * 			right away.
		 */
		void initialize();

		/**
		 * @copydoc Resource::destroy_internal.
		 */
		virtual void destroy_internal();

        /** Internal method returns whether required capabilities for this program is supported.
        */
        bool isRequiredCapabilitiesSupported(void) const;

		/// @copydoc Resource::calculateSize
		size_t calculateSize(void) const { return 0; } // TODO 

		void throwIfNotRenderThread() const;

    protected:
		friend class GpuProgramManager;

		GpuProgram(const String& source, const String& entryPoint, const String& language, 
			GpuProgramType gptype, GpuProgramProfile profile, bool isAdjacencyInfoRequired = false);

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class GpuProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	/** @} */
}

#endif
