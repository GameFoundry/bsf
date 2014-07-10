//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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
	 * @brief	Render system pipeline state that allows you to modify how an object is rasterized.
	 *			i.e. how are polygons converted to pixels.
	 *
	 * @note	Rasterizer states are immutable. Thread safe.
	 */
	class BS_CORE_EXPORT RasterizerState : public Resource
	{
	public:
		virtual ~RasterizerState() {}

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
		 * @see		RenderSystem::setScissorRect
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

		/**
		* @brief	Initializes the rasterizer state. Must be called right after construction.
		*/
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