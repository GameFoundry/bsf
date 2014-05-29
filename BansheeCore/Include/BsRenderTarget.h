#pragma once

#include "BsCorePrerequisites.h"
#include "BsString.h"
#include "BsPixelUtil.h"
#include "BsViewport.h"
#include "BsCoreObject.h"
#include "BsEvent.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Structure that contains information about
	 *			what part of the texture represents the render surface.
	 */
	struct BS_CORE_EXPORT RENDER_SURFACE_DESC
	{
		TexturePtr texture;
		UINT32 face;
		UINT32 numFaces;
		UINT32 mipLevel;
	};

	/**
	 * @brief	Render target is a frame buffer or a texture that the render
	 *			system renders to.
	 *
	 * @note	Thread safe, except where noted otherwise.
	 */
    class BS_CORE_EXPORT RenderTarget : public CoreObject
    {
    public:
		/**
		 * @brief	Frame buffer type when double-buffering is used.
		 */
		enum FrameBuffer
		{
			FB_FRONT,
			FB_BACK,
			FB_AUTO
		};

        virtual ~RenderTarget();

		/**
		 * @brief	Returns a name of the render target, used for easier identification.
		 */
        const String& getName() const { return mName; }

		/**
		 * @brief	Returns width of the render target, in pixels.
		 */
        UINT32 getWidth() const { return mWidth; }

		/**
		 * @brief	Returns height of the render target, in pixels.
		 */
        UINT32 getHeight() const { return mHeight; }

		/**
		 * @brief	Gets the number of samples used for multisampling.
		 *			(0 if multisampling is not used).
		 */
		UINT32 getMultisampleCount() const { return mMultisampleCount; }

		/**
		 * @brief	Get a render-system specific hint used for determining
		 *			multisampling type.
		 */
		const String& getMultisampleHint() const { return mMultisampleHint; }

		/**
		 * @brief	Returns true if the render target will wait for vertical sync 
		 *			before swapping buffers. This will eliminate tearing but may increase
		 *			input latency.
		 */
		bool getVSync() const { return mVSync; }

		/**
		 * @brief	Queries the render target for a custom attribute. This may be anything and is
		 *			implementation specific.
		 *
		 * @note	Core thread only.
		 */
		virtual void getCustomAttribute(const String& name, void* pData) const;

		/**
		 * @brief	Returns true if the render target is a render window.
		 */
		virtual bool isWindow() const = 0;

		/**
		 * @brief	Returns true if the render target can be used for rendering.
		 *
		 * @note	Core thread only.
		 */
		bool isActive() const { return mActive; }

		/**
		 * @brief	Returns true if pixels written to the render target will be gamma corrected.
		 */
		bool isHwGammaEnabled() const { return mHwGamma; }

		/**
		 * @brief	Makes the render target active or inactive. (e.g. for a window, it will hide or restore the window).
		 *
		 * @note	Core thread only.
		 */
		virtual void setActive(bool state) { mActive = state; }

		/**
		 * @brief	Returns render target priority. Targets with higher priority will be 
		 *			rendered before ones with lower priority.
		 */
		INT32 getPriority() const { return mPriority; }

		/**
		 * @brief	Sets a priority that determines in which orders the render targets the processed.
		 * 			
		 * @param	priority	The priority. Higher value means the target will be rendered sooner.
		 */
		void setPriority(INT32 priority) { mPriority = priority; }

        /**
         * @brief	Swaps the frame buffers to display the next frame.
         * 			
		 * @note	Core thread only.
         */
		virtual void swapBuffers() {};

		/**
		 * @brief	Copy data from the render target into the provided pixel buffer. 
		 *
		 * @param	dst		Destination buffer to copy the data to. Caller must ensure the buffer is of adequate size.
		 * @param	buffer	Which buffer is data taken from. This is irrelevant for single buffer render targets.
		 *
		 * @note	Core thread only.
		 */
		virtual void copyToMemory(const PixelData &dst, FrameBuffer buffer = FB_AUTO) = 0;

		/**
		 * @brief	Does the texture need to be vertically flipped because of different screen space coordinate systems.
		 *			(i.e. is origin top left or bottom left. Engine default is top left.)
		 */
		virtual bool requiresTextureFlipping() const = 0;

		/**
		 * @brief	Event that gets triggered whenever the render target is resized.
		 *
		 * @note	Sim thread only.
		 */
		mutable Event<void()> onResized;
    protected:
		RenderTarget();

        String mName;

        UINT32 mWidth;
        UINT32 mHeight;
        UINT32 mColorDepth;
		
		INT32 mPriority;

        bool mActive;
		bool mHwGamma;
		bool mVSync;
		UINT32 mMultisampleCount;
		String mMultisampleHint;
    };
}
