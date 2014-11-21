#pragma once

#include "BsCorePrerequisites.h"
#include "BsIReflectable.h"
#include "BsCoreObject.h"
#include "BsColor.h"
#include "BsRect2I.h"
#include "BsRect2.h"
#include "BsEvent.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Viewport provides you with a way to render to only a part of a 
	 * 			RenderTarget. It also allows you to set up color/depth/stencil
	 * 			clear values for that specific region.
	 */
	class BS_CORE_EXPORT ViewportBase
    {
    public:       
		virtual ~ViewportBase() { }

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
		INT32 getX() const;

        /**
         * @brief	Gets the actual y coordinate of the viewport in pixels, in [0, RenderTargetHeight] range.
         */
		INT32 getY() const;

		/**
         * @brief	Gets the actual width coordinate of the viewport in pixels, in [0, RenderTargetWidth] range.
         */
		INT32 getWidth() const;

		/**
         * @brief	Gets the actual height coordinate of the viewport in pixels, in [0, RenderTargetHeight] range.
         */
		INT32 getHeight() const;
               
        /**
         * @brief	Changes the area that the viewport covers.
         *
         * @note	Viewport coordinates are normalized in [0, 1] range.
         */
        void setArea(float x, float y, float width, float height);

		/**
		 * @brief	Returns actual area of the viewport, in pixels.
		 */
		Rect2I getArea() const;

		/**
		 * @brief	Returns the normalized area of the viewport.
		 *
		 * @note	Viewport coordinates are normalized in [0, 1] range.
		 */
		Rect2 getNormArea() const { return mNormArea; }

		/**
		 * @brief	Activates or deactivates clears for color, depth or stencil buffers.
		 *			Buffers will be cleared before rendering to this viewport is performed.
		 */
		void setRequiresClear(bool colorClear, bool depthClear, bool stencilClear);

		/**
		 * @brief	Sets values to clear color, depth and stencil buffers to.
		 */
		void setClearValues(const Color& clearColor, float clearDepth = 0.0f, UINT16 clearStencil = 0);

		/**
		 * @brief	Returns the color to clear the viewport color buffers to.
		 */
		const Color& getClearColor() const { return mClearColor; }

		/**
		 * @brief	Returns the value to clear the viewport depth buffers to.
		 */
		float getClearDepthValue() const { return mDepthClearValue; }

		/**
		 * @brief	Returns the value to clear the viewport stencil buffer to.
		 */
		UINT16 getClearStencilValue() const { return mStencilClearValue; }

		/**
		 * @brief	Returns true if viewport requires color clear before rendering.
		 */
		bool getRequiresColorClear() const { return mRequiresColorClear; }

		/**
		 * @brief	Returns true if viewport requires depth clear before rendering.
		 */
		bool getRequiresDepthClear() const { return mRequiresDepthClear; }

		/**
		 * @brief	Returns true if viewport requires stencil clear before rendering.
		 */
		bool getRequiresStencilClear() const { return mRequiresStencilClear; }

    protected:
        /**
         * @brief	Constructs a new viewport.
         *
         * @note	Viewport coordinates are normalized in [0, 1] range.
         */
		ViewportBase(float x = 0.0f, float y = 0.0f, float width = 1.0f, float height = 1.0f);

		/**
		 * @copydoc	CoreObject::markCoreDirty
		 */
		virtual void _markCoreDirty() { }

		/**
		 * @brief	Gets the render target width.
		 */
		virtual UINT32 getTargetWidth() const = 0;

		/**
		 * @brief	Gets the render target width.
		 */
		virtual UINT32 getTargetHeight() const = 0;

		Rect2 mNormArea;

		bool mRequiresColorClear;
		bool mRequiresDepthClear;
		bool mRequiresStencilClear;

		Color mClearColor;
		float mDepthClearValue;
		UINT16 mStencilClearValue;

		static const Color DEFAULT_CLEAR_COLOR;
    };

	/**
	 * @copydoc	ViewportBase
	 */
	class BS_CORE_EXPORT ViewportCore : public CoreObjectCore, public ViewportBase
    {
    public:       
        /**
         * @brief	Returns the render target the viewport is associated with.
         */
		SPtr<RenderTargetCore> getTarget() const { return mTarget; }

		/**
		 * @brief	Sets the render target the viewport will be associated with.
		 */
		void setTarget(const SPtr<RenderTargetCore>& target) { mTarget = target; }

		/**
		 * @copydoc	ViewportBase::ViewportBase
		 */
		static SPtr<ViewportCore> create(const SPtr<RenderTargetCore>& target, float x = 0.0f, float y = 0.0f, float width = 1.0f, float height = 1.0f);

    protected:
		friend class Viewport;

		/**
		 * @copydoc	ViewportBase::ViewportBase
		 */
		ViewportCore(const SPtr<RenderTargetCore>& target, float x = 0.0f, float y = 0.0f, float width = 1.0f, float height = 1.0f);

		/**
		 * @copydoc	CoreObject::getTargetWidth
		 */
		UINT32 getTargetWidth() const;

		/**
		 * @copydoc	CoreObject::getTargetHeight
		 */
		UINT32 getTargetHeight() const;

		/**
		 * @copydoc	CoreObject::syncToCore
		 */
		void syncToCore(const CoreSyncData& data);

		SPtr<RenderTargetCore> mTarget;
    };

	/**
	 * @copydoc	ViewportBase
	 */
	class BS_CORE_EXPORT Viewport : public IReflectable, public CoreObject, public ViewportBase
    {
    public:       
        /**
         * @brief	Returns the render target the viewport is associated with.
         */
        RenderTargetPtr getTarget() const { return mTarget; }

		/**
		 * @brief	Sets the render target the viewport will be associated with.
		 */
		void setTarget(const RenderTargetPtr& target) { mTarget = target; _markCoreDirty(); }

		/**
		 * @brief	Retrieves a core implementation of a viewport usable only from the
		 *			core thread.
		 */
		SPtr<ViewportCore> getCore() const;

		/**
		 * @copydoc	ViewportBase::ViewportBase
		 */
		static ViewportPtr create(const RenderTargetPtr& target, float x = 0.0f, float y = 0.0f, 
			float width = 1.0f, float height = 1.0f);

    protected:
		/**
		 * @copydoc	ViewportBase::ViewportBase
		 */
        Viewport(const RenderTargetPtr& target, float x = 0.0f, float y = 0.0f, float width = 1.0f, float height = 1.0f);

		/**
		 * @copydoc	CoreObject::markCoreDirty
		 */
		void _markCoreDirty();

		/**
		 * @copydoc	CoreObject::getTargetWidth
		 */
		UINT32 getTargetWidth() const;

		/**
		 * @copydoc	CoreObject::getTargetHeight
		 */
		UINT32 getTargetHeight() const;

		/**
		 * @copydoc	CoreObject::syncToCore
		 */
		CoreSyncData syncToCore(FrameAlloc* allocator);

		/**
		 * @copydoc	CoreObject::createCore
		 */
		SPtr<CoreObjectCore> createCore() const;

        RenderTargetPtr mTarget;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		Viewport();

		/**
		 * @brief	Creates an empty viewport for serialization purposes.
		 */
		static ViewportPtr createEmpty();
	public:
		friend class ViewportRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
    };
}