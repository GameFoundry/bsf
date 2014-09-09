#pragma once

#include "BsCorePrerequisites.h"
#include "BsCoreThreadAccessor.h"
#include "BsColor.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Class defining a single pass of a technique (of a material), i.e.
	 * 			a single rendering call. 
	 * 			
	 *			Pass may contain multiple GPU programs (vertex, fragment, geometry, etc.), and
	 *			a set of pipeline states (blend, rasterizer, etc.).
	 */
	class BS_CORE_EXPORT Pass : public IReflectable
    {
    public:
		Pass();
        Pass(const Pass& oth);
        Pass& operator=(const Pass& oth);
        virtual ~Pass();

        bool hasVertexProgram() const { return mVertexProgram != nullptr; }
        bool hasFragmentProgram() const { return mFragmentProgram != nullptr; }
        bool hasGeometryProgram() const { return mGeometryProgram != nullptr; }
		bool hasHullProgram() const { return mHullProgram != nullptr; }
		bool hasDomainProgram() const { return mDomainProgram != nullptr; }
		bool hasComputeProgram() const { return mComputeProgram != nullptr; }

		/**
		 * @brief	Returns true if this pass has some element of transparency.
		 */
		bool hasBlending() const;

		void setBlendState(HBlendState& blendState);
		HBlendState getBlendState() const;

		void setRasterizerState(HRasterizerState& rasterizerState);
		HRasterizerState getRasterizerState() const;

		void setDepthStencilState(HDepthStencilState& depthStencilState);
		HDepthStencilState getDepthStencilState() const;

		/**
		 * @brief	Sets the stencil reference value that is used when performing operations using the
		 * 			stencil buffer.
		 */
		void setStencilRefValue(UINT32 refValue);

		/**
		 * @brief	Gets the stencil reference value that is used when performing operations using the
		 * 			stencil buffer.
		 */
		UINT32 getStencilRefValue() const;

		void setVertexProgram(HGpuProgram gpuProgram) { mVertexProgram = gpuProgram; }
		const HGpuProgram& getVertexProgram() const { return mVertexProgram; }

		void setFragmentProgram(HGpuProgram gpuProgram) { mFragmentProgram = gpuProgram; }
		const HGpuProgram& getFragmentProgram() const { return mFragmentProgram; }

		void setGeometryProgram(HGpuProgram gpuProgram) { mGeometryProgram = gpuProgram; }
		const HGpuProgram& getGeometryProgram() const { return mGeometryProgram; }

		void setHullProgram(HGpuProgram gpuProgram) { mHullProgram = gpuProgram; }
		const HGpuProgram& getHullProgram(void) const { return mHullProgram; }

		void setDomainProgram(HGpuProgram gpuProgram) { mDomainProgram = gpuProgram;}
		const HGpuProgram& getDomainProgram(void) const { return mDomainProgram; }

		void setComputeProgram(HGpuProgram gpuProgram) { mComputeProgram = gpuProgram; }
		const HGpuProgram& getComputeProgram(void) const { return mComputeProgram; }

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

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class PassRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
    };
}