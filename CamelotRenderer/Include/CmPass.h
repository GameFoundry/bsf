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

		GpuProgramHandle mVertexProgram;
		GpuProgramHandle mFragmentProgram;
		GpuProgramHandle mGeometryProgram;
		GpuProgramHandle mHullProgram;
		GpuProgramHandle mDomainProgram;
		GpuProgramHandle mComputeProgram;
    public:
        /// Default constructor
		Pass();
        /// Copy constructor
        Pass(const Pass& oth );
        /// Operator = overload
        Pass& operator=(const Pass& oth);
        virtual ~Pass();

        bool hasVertexProgram() const { return mVertexProgram != nullptr; }
        bool hasFragmentProgram() const { return mFragmentProgram != nullptr; }
        bool hasGeometryProgram() const { return mGeometryProgram != nullptr; }
		bool hasHullProgram() const { return mHullProgram != nullptr; }
		bool hasDomainProgram() const { return mDomainProgram != nullptr; }
		bool hasComputeProgram() const { return mComputeProgram != nullptr; }

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
		void setVertexProgram(GpuProgramHandle gpuProgram) { mVertexProgram = gpuProgram; }

		/** Gets the vertex program used by this pass. */
		const GpuProgramHandle& getVertexProgram(void) const { return mVertexProgram; }

		/** Sets the details of the fragment program to use.
		*/
		void setFragmentProgram(GpuProgramHandle gpuProgram) { mFragmentProgram = gpuProgram; }
		
		/** Gets the fragment program used by this pass. */
		const GpuProgramHandle& getFragmentProgram(void) const { return mFragmentProgram; }

		/** Sets the details of the geometry program to use.
		*/
		void setGeometryProgram(GpuProgramHandle gpuProgram) { mGeometryProgram = gpuProgram; }
		
		/** Gets the geometry program used by this pass. */
		const GpuProgramHandle& getGeometryProgram(void) const { return mGeometryProgram; }

		/** Sets the details of the hull program to use.
		*/
		void setHullProgram(GpuProgramHandle gpuProgram) { mHullProgram = gpuProgram; }
		
		/** Gets the hull program used by this pass. */
		const GpuProgramHandle& getHullProgram(void) const { return mHullProgram; }

		/** Sets the details of the domain program to use.
		*/
		void setDomainProgram(GpuProgramHandle gpuProgram) { mDomainProgram = gpuProgram;}
		
		/** Gets the domain program used by this pass. */
		const GpuProgramHandle& getDomainProgram(void) const { return mDomainProgram; }

		/** Sets the details of the compute program to use.
		*/
		void setComputeProgram(GpuProgramHandle gpuProgram) { mComputeProgram = gpuProgram; }
		
		/** Gets the compute program used by this pass. */
		const GpuProgramHandle& getComputeProgram(void) const { return mComputeProgram; }

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class PassRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
    };
}