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
#include "CmRenderSystemCapabilities.h"
#include "CmResource.h"

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
		GPT_GEOMETRY_PROGRAM
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
		/// Did we encounter a compilation error?
		bool mCompileError;
		/** Record of logical to physical buffer maps. Mandatory for low-level
			programs or high-level programs which set their params the same way. 
			This is a shared pointer because if the program is recompiled and the parameters
			change, this definition will alter, but previous params may reference the old def. */
		mutable GpuLogicalBufferStructPtr mFloatLogicalToPhysical;
		/** Record of logical to physical buffer maps. Mandatory for low-level
			programs or high-level programs which set their params the same way. 
			This is a shared pointer because if the program is recompiled and the parameters
			change, this definition will alter, but previous params may reference the old def.*/
		mutable GpuLogicalBufferStructPtr mIntLogicalToPhysical;
		/** Record of logical to physical buffer maps. Mandatory for low-level
			programs or high-level programs which set their params the same way. 
			This is a shared pointer because if the program is recompiled and the parameters
			change, this definition will alter, but previous params may reference the old def.*/
		mutable GpuLogicalBufferStructPtr mSamplerLogicalToPhysical;
		/** Parameter name -> ConstantDefinition map, shared instance used by all parameter objects.
		This is a shared pointer because if the program is recompiled and the parameters
		change, this definition will alter, but previous params may reference the old def.
		*/
		mutable GpuNamedConstantsPtr mConstantDefs;

        /** Internal method returns whether required capabilities for this program is supported.
        */
        bool isRequiredCapabilitiesSupported(void) const;

		/// @copydoc Resource::calculateSize
		size_t calculateSize(void) const { return 0; } // TODO 

		/// Create the internal params logical & named mapping structures
		void createParameterMappingStructures(bool recreateIfExists = true) const;
		/// Create the internal params logical mapping structures
		void createLogicalParameterMappingStructures(bool recreateIfExists = true) const;
		/// Create the internal params named mapping structures
		void createNamedParameterMappingStructures(bool recreateIfExists = true) const;

		void throwIfNotRenderThread() const;

	public:

		GpuProgram();

		virtual ~GpuProgram();

		/**
		 * @brief	Initializes the gpu program. This must be called right after the program is constructed. 
		 * 			Called by GpuProgramManager upon creation, so usually you don't want to call this manually. 
		 * 			
		 * @note	Initialization is not done immediately, and is instead just scheduled on the render thread.
		 */
		void initialize();

		/**
		 * @brief	Performs GpuProgram initialization. Only callable from the render thread.
		 */
		virtual void initialize_internal();
		virtual void unload() {}

		/** Sets the source assembly for this program from an in-memory string.
        @remarks
            Setting this will have no effect until you (re)load the program.
        */
        virtual void setSource(const String& source);

        /** Gets the syntax code for this program e.g. arbvp1, fp20, vs_1_1 etc */
        virtual const String& getSyntaxCode(void) const { return mSyntaxCode; }

		/** Sets the syntax code for this program e.g. arbvp1, fp20, vs_1_1 etc */
		virtual void setSyntaxCode(const String& syntax);

        /** Gets the assembler source for this program. */
        virtual const String& getSource(void) const { return mSource; }
		/// Set the program type (only valid before load)
		virtual void setType(GpuProgramType t);
        /// Get the program type
        virtual GpuProgramType getType(void) const { return mType; }
		/// Sets the gpu program profile (e.g. vs_1_1, etc.). Make sure it matches the program type.
		virtual void setProfile(GpuProgramProfile profile) { mProfile = profile; }
		virtual GpuProgramProfile getProfile() const { return mProfile; }
		/// Sets the name of the entry method for the program
		virtual void setEntryPoint(const String& entryPoint) { mEntryPoint = entryPoint; }
		virtual const String& getEntryPoint() const { return mEntryPoint; }

        /** Returns the GpuProgram which should be bound to the pipeline.
        @remarks
            This method is simply to allow some subclasses of GpuProgram to delegate
            the program which is bound to the pipeline to a delegate, if required. */
        virtual GpuProgram* getBindingDelegate_internal(void) { return this; }

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
		GpuProgramParametersSharedPtr createParameters(void);
        virtual void createParameters_internal(AsyncOp& op);

		/** Returns a string that specifies the language of the gpu programs as specified
        in a material script. ie: asm, cg, hlsl, glsl
        */
        virtual const String& getLanguage(void) const;

		/** Did this program encounter a compile error when loading?
		*/
		virtual bool hasCompileError(void) const { return mCompileError; }

		/** Reset a compile error if it occurred, allowing the load to be retried
		*/
		virtual void resetCompileError(void) { mCompileError = false; }

		/// Get a read-only reference to the named constants registered for this program (manually or automatically)
		virtual const GpuNamedConstants& getNamedConstants() const { return *mConstantDefs.get(); }

		/** Get the full list of named constants.
		@note
		Only available if this parameters object has named parameters, which means either
		a high-level program which loads them, or a low-level program which has them
		specified manually.
		*/
		virtual const GpuNamedConstants& getConstantDefinitions() const { return *mConstantDefs.get(); }


    protected:
        /// Virtual method which must be implemented by subclasses, load from mSource
        virtual void loadFromSource(void) = 0;

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
