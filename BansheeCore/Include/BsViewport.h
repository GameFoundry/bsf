#pragma once

#include "BsCorePrerequisites.h"
#include "BsColor.h"
#include "BsRectI.h"
#include "BsRectF.h"
#include "BsEvent.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Viewport provides you with a way to render to only a part of a 
	 * 			RenderTarget. It also allows you to set up color/depth/stencil
	 * 			clear values for that specific region.
	 */
	class BS_CORE_EXPORT Viewport
    {
    public:       
		Viewport();
        
        /**
         * @brief	Constructs a new viewport.
         *
         * @note	Viewport coordinates are normalized in [0, 1] range.
         */
        Viewport(const RenderTargetPtr& target, float x = 0.0f, float y = 0.0f, float width = 1.0f, float height = 1.0f);
        virtual ~Viewport();

        /**
         * @brief	Returns the render target the viewport is associated with.
         */
        RenderTargetPtr getTarget() const { return mTarget; }

        /**
         * @brief	Gets the normalized x coordinate of the viewport, in [0, 1] range.
         */
        float getNormalizedX() const { return mNormArea.x; }

        /**
         * @brief	Gets the normalized y coordinate of the viewport, in [0, 1] range.
         */
        float getNormalizedY() const { return mNormArea.y; }

        /**
         * @brief	Gets the normalized width of the viewport, in [0, 1] range.
         */
        float getNormalizedWidth() const { return mNormArea.width; }

		/**
         * @brief	Gets the normalized height of the viewport, in [0, 1] range.
         */
        float getNormalizedHeight() const { return mNormArea.height; }

        /**
         * @brief	Gets the actual x coordinate of the viewport in pixels, in [0, RenderTargetWidth] range.
         */
        INT32 getX() const { return mArea.x; }

        /**
         * @brief	Gets the actual y coordinate of the viewport in pixels, in [0, RenderTargetHeight] range.
         */
        INT32 getY() const { return mArea.y; }

		/**
         * @brief	Gets the actual width coordinate of the viewport in pixels, in [0, RenderTargetWidth] range.
         */
        INT32 getWidth() const { return mArea.width; }

		/**
         * @brief	Gets the actual height coordinate of the viewport in pixels, in [0, RenderTargetHeight] range.
         */
        INT32 getHeight() const { return mArea.height; }
               
        /**
         * @brief	Changes the area that the viewport covers.
         *
         * @note	Viewport coordinates are normalized in [0, 1] range.
         */
        void setArea(float x, float y, float width, float height);

		/**
		 * @brief	Returns actual area of the viewport, in pixels.
		 */
		const RectI& getArea() const { return mArea; }

		const Color& getClearColor() const { return mClearColor; }
		void setClearColor(const Color& clearColor) { mClearColor = clearColor; }

		float getClearDepthValue() const { return mDepthClearValue; }
		void getClearDepthValue(float value) { mDepthClearValue = value; }

		UINT16 getClearStencilValue() const { return mStencilClearValue; }
		void setStencilClearValue(UINT16 value) { mStencilClearValue = value; }

		bool getRequiresColorClear() const { return mRequiresColorClear; }
		void setRequiresColorClear(bool requiresClear) { mRequiresColorClear = requiresClear; }

		bool getRequiresDepthClear() const { return mRequiresDepthClear; }
		void setRequiresDepthClear(bool requiresClear) { mRequiresDepthClear = requiresClear; }

		bool getRequiresStencilClear() const { return mRequiresStencilClear; }
		void setRequiresStencilClear(bool requiresClear) { mRequiresStencilClear = requiresClear; }

		/**
		 * @brief	Makes an exact copy of this viewport.
		 */
		Viewport clone();
    protected:
        RenderTargetPtr mTarget;

		RectF mNormArea;
		RectI mArea;

		bool mRequiresColorClear;
		bool mRequiresDepthClear;
		bool mRequiresStencilClear;

		Color mClearColor;
		float mDepthClearValue;
		UINT16 mStencilClearValue;

		static const Color DEFAULT_CLEAR_COLOR;

        void updateArea();
		void targetResized();
    };
}