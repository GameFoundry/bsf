#pragma once

#include "CmPrerequisites.h"
#include "CmCommon.h"
#include "CmColor.h"
#include "CmIReflectable.h"

namespace CamelotEngine
{
	/** Class defining a single pass of a Technique (of a Material), i.e.
        a single rendering call.
    @remarks
        Rendering can be repeated with many passes for more complex effects.
        Each pass is a programmable pass (meaning it does
        use either a vertex and fragment program, or both).
    */
	class CM_EXPORT Pass : public IReflectable
    {
    protected:
		BlendStatePtr mBlendState;
		RasterizerStatePtr mRasterizerState;
		DepthStencilStatePtr mDepthStencilState;

		// Vertex program
		GpuProgramHandle mVertexProgram;
		// Fragment program
		GpuProgramHandle mFragmentProgram;
		// Geometry program
		GpuProgramHandle mGeometryProgram;
    public:
        /// Default constructor
		Pass();
        /// Copy constructor
        Pass(const Pass& oth );
        /// Operator = overload
        Pass& operator=(const Pass& oth);
        virtual ~Pass();

        /// Returns true if this pass uses a programmable vertex pipeline
        bool hasVertexProgram(void) const { return mVertexProgram != nullptr; }
        /// Returns true if this pass uses a programmable fragment pipeline
        bool hasFragmentProgram(void) const { return mFragmentProgram != nullptr; }
        /// Returns true if this pass uses a programmable geometry pipeline
        bool hasGeometryProgram(void) const { return mGeometryProgram != nullptr; }

		/** Returns true if this pass has some element of transparency. */
		bool isTransparent(void) const;

		/**
		 * @brief	Sets a blend state used for all active render targets.
		 */
		void setBlendState(BlendStatePtr blendState);
		BlendStatePtr getBlendState() const;

		void setRasterizerState(RasterizerStatePtr rasterizerState);
		RasterizerStatePtr getRasterizerState() const;

		void setDepthStencilState(DepthStencilStatePtr depthStencilState);
		DepthStencilStatePtr getDepthStencilState() const;

		/** Sets the details of the vertex program to use.
		*/
		void setVertexProgram(GpuProgramHandle gpuProgram);

		/** Gets the vertex program used by this pass, only available after _load(). */
		const GpuProgramHandle& getVertexProgram(void) const;

		/** Sets the details of the fragment program to use.
		*/
		void setFragmentProgram(GpuProgramHandle gpuProgram);
		
		/** Gets the fragment program used by this pass, only available after _load(). */
		const GpuProgramHandle& getFragmentProgram(void) const;

		/** Sets the details of the geometry program to use.
		*/
		void setGeometryProgram(GpuProgramHandle gpuProgram);
		
		/** Gets the geometry program used by this pass, only available after _load(). */
		const GpuProgramHandle& getGeometryProgram(void) const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class PassRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
    };
}