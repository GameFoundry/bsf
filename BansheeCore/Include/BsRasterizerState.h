#pragma once

#include "BsCorePrerequisites.h"
#include "BsResource.h"

namespace BansheeEngine
{
	/**
	 * @brief	Structure that describes pipeline rasterizer state. Used for initializing
	 *			a RasterizerState.
	 *
	 * @see		RasterizerState
	 */
	struct BS_CORE_EXPORT RASTERIZER_STATE_DESC
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

	/**
	 * @brief	Information about a rasterizer state.
	 */
	class BS_CORE_EXPORT RasterizerProperties
	{
	public:
		RasterizerProperties(const RASTERIZER_STATE_DESC& desc);

		/**
		 * @brief	Polygon mode allows you to draw polygons as solid objects or as wireframe by
		 *			just drawing their edges.
		 */
		PolygonMode getPolygonMode() const { return mData.polygonMode; }

		/**
		 * @brief	Sets vertex winding order. Faces that contain vertices with this order will 
		 *			be culled and not rasterized. Used primarily for saving cycles by not rendering
		 *			backfacing faces.
		 */
		CullingMode getCullMode() const { return mData.cullMode; }

		/**
		* @brief	Represents a constant depth bias that will offset the depth values of new pixels 
		*			by the specified amount.
		*
		* @note		This is useful if you want to avoid z fighting for objects at the same or similar depth.
		*/
		int getDepthBias() const { return mData.depthBias; }

		/**
		 * @brief	Maximum depth bias value.
		 */
		float getDepthBiasClamp() const { return mData.depthBiasClamp; }

		/**
		 * @brief	Represents a dynamic depth bias that increases as the slope of the rendered polygons 
		 *			surface increases. Resulting value offsets depth values of new pixels. This offset will 
		 *			be added on top of the constant depth bias.
		 *
		 * @note	This is useful if you want to avoid z fighting for objects at the same or similar depth.
		 */
		float getSlopeScaledDepthBias() const { return mData.slopeScaledDepthBias; }

		/**
		 * @brief	If true, clipping of polygons past the far Z plane is enabled. This ensures proper
		 *			Z ordering for polygons outside of valid depth range (otherwise they all have the same
		 *			depth). It can be useful to disable if you are performing stencil operations that count on
		 *			objects having a front and a back (like stencil shadow) and don't want to clip the back.
		 */
		bool getDepthClipEnable() const { return mData.depthClipEnable; }

		/**
		 * @brief	Scissor rectangle allows you to cull all pixels outside of the scissor rectangle.
		 *			
		 * @see		RenderAPICore::setScissorRect
		 */
		bool getScissorEnable() const { return mData.scissorEnable; }

		/**
		 * @brief	Determines how are samples in multi-sample render targets handled.
		 *			If disabled all samples in the render target will be written the same value, 
		 *			and if enabled each sample will be generated separately.
		 *			
		 * @note	In order to get an antialiased image you need to both enable this option and use
		 *			a MSAA render target.
		 */
		bool getMultisampleEnable() const { return mData.multisampleEnable; }

		/**
		 * @brief	Determines should the lines be antialiased. This is separate from multi-sample
		 *			antialiasing setting as lines can be antialiased without multi-sampling.
		 *
		 * @note	This setting is usually ignored if MSAA is used, as that provides sufficient antialiasing.
		 */
		bool getAntialiasedLineEnable() const { return mData.antialiasedLineEnable; }

	protected:
		friend class RasterizerState;
		friend class RasterizerStateRTTI;

		RASTERIZER_STATE_DESC mData;
	};

	/**
	 * @brief	Core thread version of a rasterizer state.
	 *
	 * @see		RasterizerState
	 *
	 * @note	Core thread.
	 */
	class BS_CORE_EXPORT RasterizerStateCore : public CoreObjectCore
	{
	public:
		virtual ~RasterizerStateCore() {}

		/**
		 * @brief	Returns information about the rasterizer state.
		 */
		const RasterizerProperties& getProperties() const;

	protected:
		friend class RenderStateCoreManager;

		RasterizerStateCore(const RASTERIZER_STATE_DESC& desc);

		RasterizerProperties mProperties;
	};

	/**
	 * @brief	Render system pipeline state that allows you to modify how an object is rasterized.
	 *			i.e. how are polygons converted to pixels.
	 *
	 * @note	Rasterizer states are immutable. Sim thread only.
	 */
	class BS_CORE_EXPORT RasterizerState : public Resource
	{
	public:
		virtual ~RasterizerState() {}

		/**
		 * @brief	Returns information about the rasterizer state.
		 */
		const RasterizerProperties& getProperties() const;

		/**
		 * @brief	Retrieves a core implementation of the rasterizer state usable only from the
		 *			core thread.
		 */
		SPtr<RasterizerStateCore> getCore() const;

		/**
		 * @brief	Creates a new rasterizer state using the specified rasterizer state descriptor structure.
		 */
		static HRasterizerState create(const RASTERIZER_STATE_DESC& desc);

		/**
		 * @brief	Returns the default rasterizer state.
		 */
		static const RasterizerStatePtr& getDefault();

	protected:
		friend class RenderStateManager;

		RasterizerState(const RASTERIZER_STATE_DESC& desc);
		
		/**
		 * @copydoc	CoreObjectCore::createCore
		 */
		SPtr<CoreObjectCore> createCore() const;

		RasterizerProperties mProperties;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class RasterizerStateRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}