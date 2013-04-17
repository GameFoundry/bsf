#pragma once

#include "CmPrerequisites.h"
#include "CmCommonEnums.h"
#include "CmColor.h"
#include "CmIReflectable.h"

namespace CamelotFramework
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
		HBlendState mBlendState;
		HRasterizerState mRasterizerState;
		HDepthStencilState mDepthStencilState;
		UINT32 mStencilRefValue;

		HGpuProgram mVertexProgram;
		HGpuProgram mFragmentProgram;
		HGpuProgram mGeometryProgram;
		HGpuProgram mHullProgram;
		HGpuProgram mDomainProgram;
		HGpuProgram mComputeProgram;
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
		void setBlendState(HBlendState& blendState);
		HBlendState getBlendState() const;

		void setRasterizerState(HRasterizerState& rasterizerState);
		HRasterizerState getRasterizerState() const;

		void setDepthStencilState(HDepthStencilState& depthStencilState);
		HDepthStencilState getDepthStencilState() const;

		void setStencilRefValue(UINT32 refValue);
		UINT32 getStencilRefValue() const;

		/** Sets the details of the vertex program to use.
		*/
		void setVertexProgram(HGpuProgram gpuProgram) { mVertexProgram = gpuProgram; }

		/** Gets the vertex program used by this pass. */
		const HGpuProgram& getVertexProgram(void) const { return mVertexProgram; }

		/** Sets the details of the fragment program to use.
		*/
		void setFragmentProgram(HGpuProgram gpuProgram) { mFragmentProgram = gpuProgram; }
		
		/** Gets the fragment program used by this pass. */
		const HGpuProgram& getFragmentProgram(void) const { return mFragmentProgram; }

		/** Sets the details of the geometry program to use.
		*/
		void setGeometryProgram(HGpuProgram gpuProgram) { mGeometryProgram = gpuProgram; }
		
		/** Gets the geometry program used by this pass. */
		const HGpuProgram& getGeometryProgram(void) const { return mGeometryProgram; }

		/** Sets the details of the hull program to use.
		*/
		void setHullProgram(HGpuProgram gpuProgram) { mHullProgram = gpuProgram; }
		
		/** Gets the hull program used by this pass. */
		const HGpuProgram& getHullProgram(void) const { return mHullProgram; }

		/** Sets the details of the domain program to use.
		*/
		void setDomainProgram(HGpuProgram gpuProgram) { mDomainProgram = gpuProgram;}
		
		/** Gets the domain program used by this pass. */
		const HGpuProgram& getDomainProgram(void) const { return mDomainProgram; }

		/** Sets the details of the compute program to use.
		*/
		void setComputeProgram(HGpuProgram gpuProgram) { mComputeProgram = gpuProgram; }
		
		/** Gets the compute program used by this pass. */
		const HGpuProgram& getComputeProgram(void) const { return mComputeProgram; }

		/**
		 * @brief	Makes this pass active. Anything rendered after this command will use this pass.
		 */
		void activate(DeferredRenderContextPtr& renderContext) const;

		/**
		 * @brief	Applies specified parameters to the active pass. 
		 */
		void bindParameters(DeferredRenderContextPtr& renderContext, const PassParametersPtr& params) const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class PassRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
    };
}