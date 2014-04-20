#pragma once

#include "CmPrerequisites.h"
#include "CmCommonEnums.h"
#include "CmResource.h"

namespace BansheeEngine
{
	struct CM_EXPORT RASTERIZER_STATE_DESC
	{
		RASTERIZER_STATE_DESC()
			: polygonMode(PM_SOLID)
			, cullMode(CULL_COUNTERCLOCKWISE)
			, depthBias(0)
			, depthBiasClamp(0.0f)
			, slopeScaledDepthBias(0.0f)
			, depthClipEnable(true)
			, scissorEnable(false)
			, multisampleEnable(true)
			, antialiasedLineEnable(false)
		{ }

		PolygonMode polygonMode;
		CullingMode cullMode;

		int depthBias;
		float depthBiasClamp;
		float slopeScaledDepthBias;

		bool depthClipEnable;
		bool scissorEnable;
		bool multisampleEnable;
		bool antialiasedLineEnable;
	};

	// TODO Low priority - Write doc explaining various states
	class CM_EXPORT RasterizerState : public Resource
	{
	public:
		virtual ~RasterizerState() {}

		PolygonMode getPolygonMode() const { return mData.polygonMode; }
		CullingMode getCullMode() const { return mData.cullMode; }

		int getDepthBias() const { return mData.depthBias; }
		float getDepthBiasClamp() const { return mData.depthBiasClamp; }
		float getSlopeScaledDepthBias() const { return mData.slopeScaledDepthBias; }

		bool getDepthClipEnable() const { return mData.depthClipEnable; }
		bool getScissorEnable() const { return mData.scissorEnable; }
		bool getMultisampleEnable() const { return mData.multisampleEnable; }
		bool getAntialiasedLineEnable() const { return mData.antialiasedLineEnable; }

		static HRasterizerState create(const RASTERIZER_STATE_DESC& desc);

		/**
		 * @brief	Returns the default rasterizer state;
		 */
		static const RasterizerStatePtr& getDefault();

	protected:
		friend class RenderStateManager;

		virtual void initialize(const RASTERIZER_STATE_DESC& desc);
		RASTERIZER_STATE_DESC mData;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class RasterizerStateRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}