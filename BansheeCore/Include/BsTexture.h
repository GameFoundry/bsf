//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsGpuResource.h"
#include "BsHardwareBuffer.h"
#include "BsPixelUtil.h"
#include "BsTextureView.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Properties that describe how is the texture used.
	 */
    enum TextureUsage
    {
		TU_STATIC = GBU_STATIC, /**< A regular texture that is not often or ever updated from the CPU. */
		TU_DYNAMIC = GBU_DYNAMIC, /**< A regular texture that is often updated by the CPU. */
		TU_RENDERTARGET = 0x200, /**< Texture used for rendering by the GPU. */
		TU_DEPTHSTENCIL = 0x400, /**< Texture used as a depth/stencil buffer by the GPU. */
		TU_DEFAULT = TU_STATIC
    };

	/**
	 * @brief	Different texture types.
	 */
    enum TextureType
    {
		TEX_TYPE_1D = 1, /**< One dimensional texture. Just a row of pixels. */
		TEX_TYPE_2D = 2, /**< Two dimensional texture. */
		TEX_TYPE_3D = 3, /**< Three dimensional texture. */
		TEX_TYPE_CUBE_MAP = 4 /**< Texture consisting out of six 2D textures describing an inside of a cube. Allows special sampling. */
    };

	/**
	 * @brief	Mipmap options.
	 */
	enum TextureMipmap
	{
		MIP_UNLIMITED = 0x7FFFFFFF /**< Create all mip maps down to 1x1. */
	};

	/**
	 * @brief	Abstract class representing a texture. Specific render systems have their
	 *			own Texture implementations. Internally represented as one or more surfaces
	 *			with pixels in a certain number of dimensions, backed by a hardware buffer.
	 *
	 * @note	Core thread unless specified otherwise.
	 */
    class BS_CORE_EXPORT Texture : public GpuResource
    {
    public:
        /**
         * @brief	Gets the type of texture.
         * 			
		 * @note	Thread safe.
         */
        virtual TextureType getTextureType() const { return mTextureType; }

        /**
		 * @brief	Gets the number of mipmaps to be used for this texture. This number excludes the top level  
		 *			map (which is always assumed to be present).
         *
         * @note	Thread safe.
         */
        virtual UINT32 getNumMipmaps() const {return mNumMipmaps;}

		/**
		 * @brief	Gets whether this texture will be set up so that on sampling it,
		 *			hardware gamma correction is applied.
		 *
		 * @note	Thread safe.
		 */
		virtual bool isHardwareGammaEnabled() const { return mHwGamma; }

		/**
		 * @brief	Gets the number of samples used for multisampling.
		 *			(0 if multisampling is not used).
		 *
		 * @note	Thread safe.
		 */
		virtual UINT32 getMultisampleCount() const { return mMultisampleCount; }

		/**
		 * @brief	Get a render-system specific hint used for determining
		 *			multisampling type.
		 *
		 * @note	Thread safe.
		 */
		virtual const String& getMultisampleHint() const { return mMultisampleHint; }

        /**
         * @brief	Returns the height of the texture.
         *
         * @note	Thread safe.
         */
        virtual UINT32 getHeight() const { return mHeight; }

        /**
         * @brief	Returns the width of the texture.
         *
         * @note	Thread safe.
         */
        virtual UINT32 getWidth() const { return mWidth; }

        /**
         * @brief	Returns the depth of the texture (only applicable for 3D textures).
         *
         * @note	Thread safe.
         */
        virtual UINT32 getDepth() const { return mDepth; }

        /**
         * @brief	Returns texture usage (TextureUsage) of this texture.
         *
         * @note	Thread safe.
         */
        virtual int getUsage() const { return mUsage; }

		/**
		 * @brief	Returns the pixel format for the texture surface.
		 *
		 * @note	Thread safe.
		 */
		virtual PixelFormat getFormat() const { return mFormat; }

        /**
         * @brief	Returns true if the texture has an alpha layer.
         *
         * @note	Thread safe.
         */
        virtual bool hasAlpha() const;

        /**
         * @brief	Return the number of faces this texture has.
         *
         * @note	Thread safe.
         */
        virtual UINT32 getNumFaces() const;

		/**
		 * @copydoc GpuResource::writeSubresource
		 */
		virtual void writeSubresource(UINT32 subresourceIdx, const GpuResourceData& data, bool discardEntireBuffer);

		/**
		 * @copydoc GpuResource::readSubresource
		 */
		virtual void readSubresource(UINT32 subresourceIdx, GpuResourceData& data);

		/**
		 * @brief	Allocates a buffer you may use for storage when reading or writing a sub-resource. You
		 * 			need to allocate such a buffer if you are calling "readSubresource".
		 *
		 *			You can retrieve a sub-resource index by calling "mapToSubresourceIdx".
		 * 			
		 * @note	Thread safe.
		 */
		PixelDataPtr allocateSubresourceBuffer(UINT32 subresourceIdx) const;

		/**
		 * @brief	Maps a sub-resource index to an exact face and mip level. Sub-resource indexes
		 * 			are used when reading or writing to the resource.
		 * 			
		 * @note	Sub-resource index is only valid for the instance it was created on. You cannot use a sub-resource
		 * 			index from a different texture and expect to get valid result. Modifying the resource so the number
		 * 			of sub-resources changes invalidates all sub-resource indexes.
		 * 			
		 *			Thread safe.
		 */
		void mapFromSubresourceIdx(UINT32 subresourceIdx, UINT32& face, UINT32& mip) const;

		/**
		 * @brief	Map a face and a mip level to a sub-resource index you can use for updating or reading
		 * 			a specific sub-resource.
		 * 			
		 * @note	Generated sub-resource index is only valid for the instance it was created on. Modifying the resource so the number
		 * 			of sub-resources changes, invalidates all sub-resource indexes.
		 * 			
		 *			Thread safe.
		 */
		UINT32 mapToSubresourceIdx(UINT32 face, UINT32 mip) const;

		/**
		 * @brief	Locks the buffer for reading or writing.
		 *
		 * @param	options 	Options for controlling what you may do with the locked data.
		 * @param	mipLevel	(optional) Mipmap level to lock.
		 * @param	face		(optional) Texture face to lock.
		 * 						
		 * @return	Pointer to the buffer data. Only valid until you call unlock.
		 * 			
		 * @note	If you are just reading or writing one block of data use
		 * 			readData/writeData methods as they can be much faster in certain situations.
		 * 			
		 *			Core thread only.
		 */
		PixelData lock(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0);

		/**
		 * @brief	Unlocks a previously locked buffer. After the buffer is unlocked,
		 * 			any data returned by lock becomes invalid.
		 * 			
		 * @note	Core thread only.
		 */
		void unlock();

		/**
		 * @brief	Copies the contents of this texture to another texture. Texture format
		 * 			and size must match.
		 * 			
		 * @note	Core thread only.
		 */
		void copy(TexturePtr& target);

		/**
		 * @brief	Reads data from the texture buffer into the provided buffer.
		 * 		  
		 * @param	dest	Previously allocated buffer to read data into.
		 * @param	mipLevel	(optional) Mipmap level to read from.
		 * @param	face		(optional) Texture face to read from.
		 *
		 * @note	Core thread only.
		 */
		virtual void readData(PixelData& dest, UINT32 mipLevel = 0, UINT32 face = 0) = 0;

		/**
		 * @brief	Writes data from the provided buffer into the texture buffer.
		 * 		  
		 * @param	dest		Buffer to retrieve the data from.
		 * @param	mipLevel	(optional) Mipmap level to write into.
		 * @param	face		(optional) Texture face to write into.
		 * @param	discardWholeBuffer (optional) If true any existing texture data will be discard. This can
		 *							    improve performance of the write operation.
		 *
		 * @note	Core thread only.
		 */
		virtual void writeData(const PixelData& src, UINT32 mipLevel = 0, UINT32 face = 0, bool discardWholeBuffer = false) = 0;

		/**
		 * @brief	Returns a dummy 2x2 texture. Don't modify the returned texture.
		 * 			
		 * @note	Thread safe.
		 */
		static const HTexture& dummy();

		/**
		 * @brief	Returns true if the texture can be bound to a shader.
		 *
		 * @note	This is only false for some rare special cases. (e.g. AA render texture in DX9)
		 *			Internal method.
		 */
		virtual bool isBindableAsShaderResource() const { return true; }

		/************************************************************************/
		/* 								TEXTURE VIEW                      		*/
		/************************************************************************/

		/**
		 * @brief	Requests a texture view for the specified mip and array ranges. Returns an existing view of one for
		 *			the specified ranges already exists, otherwise creates a new one. You must release all views
		 *			by calling "releaseView" when done.
		 */
		static TextureViewPtr requestView(TexturePtr texture, UINT32 mostDetailMip, UINT32 numMips, UINT32 firstArraySlice, UINT32 numArraySlices, GpuViewUsage usage);

		/**
		 * @brief	Releases the view. View won't actually get destroyed until all references to it are released.
		 */
		static void releaseView(TextureViewPtr view);

		/************************************************************************/
		/* 								STATICS		                     		*/
		/************************************************************************/

		/**
		 * @brief	Creates a new empty texture.
		 *
		 * @param	texType				Type of the texture.
		 * @param	width				Width of the texture in pixels.
		 * @param	height				Height of the texture in pixels.
		 * @param	depth				Depth of the texture in pixels (Must be 1 for 2D textures).
		 * @param	numMips				Number of mip-maps the texture has. This number excludes the full resolution map.
		 * @param	format				Format of the pixels.
		 * @param	usage				Describes how we plan on using the texture in the pipeline.
		 * @param	hwGammaCorrection	If true the texture data is assumed to have been gamma corrected and will be
		 *								converted back to linear space when sampled on GPU.
		 * @param	multisampleCount	If higher than 1, texture containing multiple samples per pixel is created.
		 * @param	multisampleHint		Hint about what kind of multisampling to use. Render system specific.
		 */
		static HTexture create(TextureType texType, UINT32 width, UINT32 height, UINT32 depth,
			int numMips, PixelFormat format, int usage = TU_DEFAULT,
			bool hwGammaCorrection = false, UINT32 multisampleCount = 0, const String& multisampleHint = StringUtil::BLANK);


		/**
		 * @brief	Creates a new empty texture.
		 *
		 * @param	texType				Type of the texture.
		 * @param	width				Width of the texture in pixels.
		 * @param	height				Height of the texture in pixels.
		 * @param	numMips				Number of mip-maps the texture has. This number excludes the full resolution map.
		 * @param	format				Format of the pixels.
		 * @param	usage				Describes how we plan on using the texture in the pipeline.
		 * @param	hwGammaCorrection	If true the texture data is assumed to have been gamma corrected and will be
		 *								converted back to linear space when sampled on GPU.
		 * @param	multisampleCount	If higher than 1, texture containing multiple samples per pixel is created.
		 * @param	multisampleHint		Hint about what kind of multisampling to use. Render system specific.
		 */
		static HTexture create(TextureType texType, UINT32 width, UINT32 height, int numMips,
			PixelFormat format, int usage = TU_DEFAULT,
			bool hwGammaCorrection = false, UINT32 multisampleCount = 0, const String& multisampleHint = StringUtil::BLANK);

		/**
		 * @copydoc	create(TextureType, UINT32, UINT32, UINT32, int, PixelFormat, int, bool, UINT32, const String&)
		 *
		 * @note	Internal method. Creates a texture pointer without a handle. Use "create" for normal usage.
		 */
		static TexturePtr _createPtr(TextureType texType, UINT32 width, UINT32 height, UINT32 depth,
			int num_mips, PixelFormat format, int usage = TU_DEFAULT,
			bool hwGammaCorrection = false, UINT32 multisampleCount = 0, const String& multisampleHint = StringUtil::BLANK);

		/**
		 * @copydoc	create(TextureType, UINT32, UINT32, int, PixelFormat, int, bool, UINT32, const String&)
		 *
		 * @note	Internal method. Creates a texture pointer without a handle. Use "create" for normal usage.
		 */
		static TexturePtr _createPtr(TextureType texType, UINT32 width, UINT32 height, int num_mips,
			PixelFormat format, int usage = TU_DEFAULT,
			bool hwGammaCorrection = false, UINT32 multisampleCount = 0, const String& multisampleHint = StringUtil::BLANK);

	protected:
		/************************************************************************/
		/* 								TEXTURE VIEW                      		*/
		/************************************************************************/

		/**
		 * @brief	Creates a new empty/undefined texture view.
		 */
		virtual TextureViewPtr createView();

		/**
		 * @brief	Releases all internal texture view references. Views won't get destroyed if there are external references still held.
		 */
		void clearBufferViews();

		/**
		 * @brief	Holds a single texture view with a usage reference count.
		 */
		struct TextureViewReference
		{
			TextureViewReference(TextureViewPtr _view)
				:view(_view), refCount(0)
			{ }

			TextureViewPtr view;
			UINT32 refCount;
		};

		UnorderedMap<TEXTURE_VIEW_DESC, TextureViewReference*, TextureView::HashFunction, TextureView::EqualFunction> mTextureViews;

    protected:
		friend class TextureManager;

		Texture();

		/**
		 * @copydoc	GpuResource::initialize
		 */
		void initialize(TextureType textureType, UINT32 width, UINT32 height, UINT32 depth, UINT32 numMipmaps, 
			PixelFormat format, int usage, bool hwGamma, UINT32 multisampleCount, const String& multisampleHint);

		/**
		 * @copydoc	lock
		 */
		virtual PixelData lockImpl(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0) = 0;

		/**
		 * @copydoc	unlock
		 */
		virtual void unlockImpl() = 0;

		/**
		 * @copydoc	copy
		 */
		virtual void copyImpl(TexturePtr& target) = 0;

		/**
		 * @copydoc	Resource::calculateSize
		 */
		UINT32 calculateSize() const;

	protected:
		UINT32 mHeight; // Immutable
		UINT32 mWidth; // Immutable
		UINT32 mDepth; // Immutable

		UINT32 mNumMipmaps; // Immutable
		bool mHwGamma; // Immutable
		UINT32 mMultisampleCount; // Immutable
		String mMultisampleHint; // Immutable

		TextureType mTextureType; // Immutable
		PixelFormat mFormat; // Immutable
		int mUsage; // Immutable

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class TextureRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
    };
}