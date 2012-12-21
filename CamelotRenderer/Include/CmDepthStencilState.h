#pragma once

#include "CmPrerequisites.h"
#include "CmCommon.h"

namespace CamelotEngine
{
	struct CM_EXPORT DEPTH_STENCIL_DESC
	{
		DEPTH_STENCIL_DESC()
			: depthReadEnable(true)
			, depthWriteEnable(true)
			, depthComparisonFunc(CMPF_LESS)
			, stencilEnable(false)
			, stencilReadMask(0xFF)
			, stencilWriteMask(0xFF)
			, frontStencilFailOp(SOP_KEEP)
			, frontStencilZFailOp(SOP_KEEP)
			, frontStencilPassOp(SOP_KEEP)
			, frontStencilComparisonFunc(CMPF_ALWAYS_PASS)
			, backStencilFailOp(SOP_KEEP)
			, backStencilZFailOp(SOP_KEEP)
			, backStencilPassOp(SOP_KEEP)
			, backStencilComparisonFunc(CMPF_ALWAYS_PASS)
		{ }

		bool depthReadEnable;
		bool depthWriteEnable;
		CompareFunction depthComparisonFunc;

		bool stencilEnable;
		UINT8 stencilReadMask;
		UINT8 stencilWriteMask;

		StencilOperation frontStencilFailOp;
		StencilOperation frontStencilZFailOp;
		StencilOperation frontStencilPassOp;
		CompareFunction frontStencilComparisonFunc;

		StencilOperation backStencilFailOp;
		StencilOperation backStencilZFailOp;
		StencilOperation backStencilPassOp;
		CompareFunction backStencilComparisonFunc;
	};

	// TODO Low priority - Write doc explaining various states
	class CM_EXPORT DepthStencilState
	{
	public:
		virtual ~DepthStencilState() {}

		bool getDepthReadEnable() const { return mData.depthReadEnable; }
		bool getDepthWriteEnable() const { return mData.depthWriteEnable; }
		CompareFunction getDepthComparisonFunc() const { return mData.depthComparisonFunc; }

		bool getStencilEnable() const { return mData.stencilEnable; }
		UINT8 getStencilReadMask() const { return mData.stencilReadMask; }
		UINT8 getStencilWriteMask() const { return mData.stencilWriteMask; }

		StencilOperation getStencilFrontFailOp() const { return mData.frontStencilFailOp; }
		StencilOperation getStencilFrontZFailOp() const { return mData.frontStencilZFailOp; }
		StencilOperation getStencilFrontPassOp() const { return mData.frontStencilPassOp; }
		CompareFunction getStencilFrontCompFunc() const { return mData.frontStencilComparisonFunc; }

		StencilOperation getStencilBackFailOp() const { return mData.backStencilFailOp; }
		StencilOperation getStencilBackZFailOp() const { return mData.backStencilZFailOp; }
		StencilOperation getStencilBackPassOp() const { return mData.backStencilPassOp; }
		CompareFunction getStencilBackCompFunc() const { return mData.backStencilComparisonFunc; }

		void setStencilRefValue(UINT32 refValue) { mStencilRefValue = refValue; }
		UINT32 getStencilRefValue() const { return mStencilRefValue; }

		static DepthStencilStatePtr create(const DEPTH_STENCIL_DESC& desc);
	private:
		friend class RenderStateManager;
		DepthStencilState();

		virtual void initialize(const DEPTH_STENCIL_DESC& desc);

		DEPTH_STENCIL_DESC mData;
		UINT32 mStencilRefValue;
	};
}