#pragma once

#include "BsCorePrerequisites.h"
#include "BsCoreThreadAccessor.h"
#include "BsColor.h"
#include "BsIReflectable.h"
#include "BsCoreObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains all data used by a pass, templated
	 *			so it may contain both core and sim thread data.
	 */
	template<bool Core>
	struct PassData
	{ };

	template<>
	struct PassData < false >
	{
		typedef HBlendState BlendStateType;
		typedef HRasterizerState RasterizerStateType;
		typedef HDepthStencilState DepthStencilStateType;
		typedef HGpuProgram GpuProgramType;

		BlendStateType mBlendState;
		RasterizerStateType mRasterizerState;
		DepthStencilStateType mDepthStencilState;
		UINT32 mStencilRefValue;

		GpuProgramType mVertexProgram;
		GpuProgramType mFragmentProgram;
		GpuProgramType mGeometryProgram;
		GpuProgramType mHullProgram;
		GpuProgramType mDomainProgram;
		GpuProgramType mComputeProgram;
	};

	template<>
	struct PassData < true >
	{
		typedef SPtr<BlendStateCore> BlendStateType;
		typedef SPtr<RasterizerStateCore> RasterizerStateType;
		typedef SPtr<DepthStencilStateCore> DepthStencilStateType;
		typedef SPtr<GpuProgramCore> GpuProgramType;

		BlendStateType mBlendState;
		RasterizerStateType mRasterizerState;
		DepthStencilStateType mDepthStencilState;
		UINT32 mStencilRefValue;

		GpuProgramType mVertexProgram;
		GpuProgramType mFragmentProgram;
		GpuProgramType mGeometryProgram;
		GpuProgramType mHullProgram;
		GpuProgramType mDomainProgram;
		GpuProgramType mComputeProgram;
	};

	/**
	 * @brief	Class defining a single pass of a technique (of a material), i.e.
	 * 			a single rendering call.
	 *
	 *			Pass may contain multiple GPU programs (vertex, fragment, geometry, etc.), and
	 *			a set of pipeline states (blend, rasterizer, etc.).
	 */
	class BS_CORE_EXPORT PassBase
	{
	public:
		virtual ~PassBase() { }

	protected:
		/**
		 * @copydoc	CoreObject::markCoreDirty
		 */
		virtual void _markCoreDirty() { }

		/**
		 * @brief	Returns true if the provided blend state has some element of transparency.
		 */
		static bool hasBlending(const HBlendState& blendState);

		/**
		 * @brief	Returns true if the provided blend state has some element of transparency.
		 */
		static bool hasBlending(const SPtr<BlendStateCore>& blendState);
	};

	/**
	 *  @see	PassBase
	 *
	 * @note	Templated so it can be used for both core and non-core versions of a pass.
	 */
	template<bool Core>
	class TPass : public PassBase
    {
    public:
		typedef typename PassData<Core>::BlendStateType BlendStateType;
		typedef typename PassData<Core>::RasterizerStateType RasterizerStateType;
		typedef typename PassData<Core>::DepthStencilStateType DepthStencilStateType;
		typedef typename PassData<Core>::GpuProgramType GpuProgramType;

		virtual ~TPass() { }

        bool hasVertexProgram() const { return mData.mVertexProgram != nullptr; }
		bool hasFragmentProgram() const { return mData.mFragmentProgram != nullptr; }
		bool hasGeometryProgram() const { return mData.mGeometryProgram != nullptr; }
		bool hasHullProgram() const { return mData.mHullProgram != nullptr; }
		bool hasDomainProgram() const { return mData.mDomainProgram != nullptr; }
		bool hasComputeProgram() const { return mData.mComputeProgram != nullptr; }

		/**
		 * @brief	Returns true if this pass has some element of transparency.
		 */
		bool hasBlending() const { return PassBase::hasBlending(mData.mBlendState); }

		void setBlendState(const BlendStateType& blendState) { mData.mBlendState = blendState; _markCoreDirty(); }
		BlendStateType getBlendState() const { return mData.mBlendState; }

		void setRasterizerState(const RasterizerStateType& rasterizerState) { mData.mRasterizerState = rasterizerState; _markCoreDirty(); }
		RasterizerStateType getRasterizerState() const { return mData.mRasterizerState; }

		void setDepthStencilState(const DepthStencilStateType& depthStencilState) { mData.mDepthStencilState = depthStencilState; _markCoreDirty(); }
		DepthStencilStateType getDepthStencilState() const { return mData.mDepthStencilState; }

		/**
		 * @brief	Sets the stencil reference value that is used when performing operations using the
		 * 			stencil buffer.
		 */
		void setStencilRefValue(UINT32 refValue) { mData.mStencilRefValue = refValue; _markCoreDirty(); }

		/**
		 * @brief	Gets the stencil reference value that is used when performing operations using the
		 * 			stencil buffer.
		 */
		UINT32 getStencilRefValue() const { return mData.mStencilRefValue; }

		void setVertexProgram(const GpuProgramType& gpuProgram) { mData.mVertexProgram = gpuProgram; _markCoreDirty(); }
		const GpuProgramType& getVertexProgram() const { return mData.mVertexProgram; }

		void setFragmentProgram(const GpuProgramType& gpuProgram) { mData.mFragmentProgram = gpuProgram; _markCoreDirty(); }
		const GpuProgramType& getFragmentProgram() const { return mData.mFragmentProgram; }

		void setGeometryProgram(const GpuProgramType& gpuProgram) { mData.mGeometryProgram = gpuProgram; _markCoreDirty(); }
		const GpuProgramType& getGeometryProgram() const { return mData.mGeometryProgram; }

		void setHullProgram(const GpuProgramType& gpuProgram) { mData.mHullProgram = gpuProgram; _markCoreDirty(); }
		const GpuProgramType& getHullProgram(void) const { return mData.mHullProgram; }

		void setDomainProgram(const GpuProgramType& gpuProgram) { mData.mDomainProgram = gpuProgram; _markCoreDirty(); }
		const GpuProgramType& getDomainProgram(void) const { return mData.mDomainProgram; }

		void setComputeProgram(const GpuProgramType& gpuProgram) { mData.mComputeProgram = gpuProgram; _markCoreDirty(); }
		const GpuProgramType& getComputeProgram(void) const { return mData.mComputeProgram; }

	protected:
		TPass()
		{
			mData.mStencilRefValue = 0;
		}

		PassData<Core> mData;
    };

	/**
	 * @copydoc	PassBase
	 *
	 * @note	Core thread.
	 */
	class BS_CORE_EXPORT PassCore : public CoreObjectCore, public TPass<true>
    {
    public:
		virtual ~PassCore() { }

		/**
		 * @brief	Creates a new empty pass.
		 */
		static SPtr<PassCore> create();

	protected:
		friend class Pass;
		friend class TechniqueCore;

		PassCore() { }

		/**
		 * @copydoc	CoreObjectCore::syncToCore
		 */
		void syncToCore(const CoreSyncData& data);
    };

	/**
	 * @copydoc	PassBase
	 *
	 * @note	Sim thread.
	 */
	class BS_CORE_EXPORT Pass : public IReflectable, public CoreObject, public TPass<false>
    {
    public:
		virtual ~Pass() { }

		/**
		 * @brief	Retrieves an implementation of a pass usable only from the
		 *			core thread.
		 */
		SPtr<PassCore> getCore() const;

		/**
		 * @brief	Creates a new empty pass.
		 */
		static PassPtr create();

	protected:
		friend class Technique;

		Pass() { }

		/**
		 * @copydoc	CoreObject::createCore
		 */
		SPtr<CoreObjectCore> createCore() const;

		/**
		 * @copydoc	CoreObject::markCoreDirty
		 */
		void _markCoreDirty();

		/**
		 * @copydoc	CoreObject::syncToCore
		 */
		CoreSyncData syncToCore(FrameAlloc* allocator);

		/**
		 * @brief	Creates a new empty pass but doesn't initialize it.
		 */
		static PassPtr createEmpty();

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class PassRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
    };
}