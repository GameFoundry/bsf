#pragma once

#include "BsCorePrerequisites.h"
#include "BsResource.h"
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
		TU_LOADSTORE = 0x800, /**< Texture that allows load/store operations from the GPU program. */
		TU_CPUCACHED = 0x1000, /**< All texture data will also be cached in system memory. */
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
	 * @brief	Contains various information about a texture
	 */
	class BS_CORE_EXPORT TextureProperties
	{
	public:
		TextureProperties();
		TextureProperties(TextureType textureType, UINT32 width, UINT32 height, UINT32 depth, UINT32 numMipmaps,
			PixelFormat format, int usage, bool hwGamma, UINT32 multisampleCount);

		/**
         * @brief	Gets the type of texture.
         */
        TextureType getTextureType() const { return mTextureType; }

        /**
		 * @brief	Gets the number of mipmaps to be used for this texture. This number excludes the top level  
		 *			map (which is always assumed to be present).
         */
        UINT32 getNumMipmaps() const {return mNumMipmaps;}

		/**
		 * @brief	Gets whether this texture will be set up so that on sampling it,
		 *			hardware gamma correction is applied.
		 */
		bool isHardwareGammaEnabled() const { return mHwGamma; }

		/**
		 * @brief	Gets the number of samples used for multisampling.
		 *			(0 if multisampling is not used).
		 */
		UINT32 getMultisampleCount() const { return mMultisampleCount; }

        /**
         * @brief	Returns the height of the texture.
         */
        UINT32 getHeight() const { return mHeight; }

        /**
         * @brief	Returns the width of the texture.
         */
        UINT32 getWidth() const { return mWidth; }

        /**
         * @brief	Returns the depth of the texture (only applicable for 3D textures).
         */
        UINT32 getDepth() const { return mDepth; }

        /**
         * @brief	Returns texture usage (TextureUsage) of this texture.
         */
        int getUsage() const { return mUsage; }

		/**
		 * @brief	Returns the pixel format for the texture surface.
		 */
		PixelFormat getFormat() const { return mFormat; }

        /**
         * @brief	Returns true if the texture has an alpha layer.
         */
        bool hasAlpha() const;

        /**
         * @brief	Return the number of faces this texture has.
         */
        UINT32 getNumFaces() const;

		/**
		 * @brief	Maps a sub-resource index to an exact face and mip level. Sub-resource indexes
		 * 			are used when reading or writing to the resource.
		 * 			
		 * @note	Sub-resource index is only valid for the instance it was created on. You cannot use a sub-resource
		 * 			index from a different texture and expect to get valid result. Modifying the resource so the number
		 * 			of sub-resources changes invalidates all sub-resource indexes.
		 */
		void mapFromSubresourceIdx(UINT32 subresourceIdx, UINT32& face, UINT32& mip) const;

		/**
		 * @brief	Map a face and a mip level to a sub-resource index you can use for updating or reading
		 * 			a specific sub-resource.
		 * 			
		 * @note	Generated sub-resource index is only valid for the instance it was created on. Modifying the resource so the number
		 * 			of sub-resources changes, invalidates all sub-resource indexes.
		 */
		UINT32 mapToSubresourceIdx(UINT32 face, UINT32 mip) const;

	protected:
		friend class TextureRTTI;

		UINT32 mHeight;
		UINT32 mWidth;
		UINT32 mDepth;

		UINT32 mNumMipmaps;
		bool mHwGamma;
		UINT32 mMultisampleCount;

		TextureType mTextureType;
		PixelFormat mFormat;
		int mUsage;
	};

	/**
	 * @brief	Core thread version of a Texture.
	 *
	 * @see		Texture
	 *
	 * @note	Core thread.
	 */
	class BS_CORE_EXPORT TextureCore : public CoreObjectCore
	{
	public:
		TextureCore(TextureType textureType, UINT32 width, UINT32 height, UINT32 depth, UINT32 numMipmaps,
			PixelFormat format, int usage, bool hwGamma, UINT32 multisampleCount);
		virtual ~TextureCore() {}

		/**
		 * @brief	Updates a part of the texture with the provided data.
		 *
		 * @param	subresourceIdx		Index of the subresource to update, if the texture has more than one.
		 * @param	data				Data to update the texture with.
		 * @param	discardEntireBuffer When true the existing contents of the resource you are updating will be discarded. This can make the
		 * 								operation faster. Resources with certain buffer types might require this flag to be in a specific state
		 * 								otherwise the operation will fail.
		 */
		virtual void writeSubresource(UINT32 subresourceIdx, const PixelData& data, bool discardEntireBuffer);

		/**
		 * @brief	Reads a part of the current resource into the provided "data" parameter.
		 * 			Data buffer needs to be pre-allocated.
		 *
		 * @param	subresourceIdx		Index of the subresource to update, if the texture has more than one.
		 * @param	data				Buffer that will receive the data. Should be allocated with "allocateSubresourceBuffer"
		 *								to ensure it is of valid type and size.
		 */
		virtual void readSubresource(UINT32 subresourceIdx, PixelData& data);

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
		 */
		PixelData lock(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0);

		/**
		 * @brief	Unlocks a previously locked buffer. After the buffer is unlocked,
		 * 			any data returned by lock becomes invalid.
		 */
		void unlock();

		/**
		 * @brief	Copies the contents a subresource in this texture to another texture. 
		 *			Texture format and size of the subresource must match.
		 *
		 *			You are allowed to copy from a multisampled to non-multisampled
		 *			surface, which will resolve the multisampled surface before copying.
		 *
		 * @param	srcSubresourceIdx	Index of the subresource to copy from.
		 * @param	destSubresourceIdx	Index of the subresource to copy to.
		 * @param	target				Texture that contains the destination subresource.
		 */
		void copy(UINT32 srcSubresourceIdx, UINT32 destSubresourceIdx, const SPtr<TextureCore>& target);

		/**
		 * @brief	Reads data from the texture buffer into the provided buffer.
		 * 		  
		 * @param	dest	Previously allocated buffer to read data into.
		 * @param	mipLevel	(optional) Mipmap level to read from.
		 * @param	face		(optional) Texture face to read from.
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
		 */
		virtual void writeData(const PixelData& src, UINT32 mipLevel = 0, UINT32 face = 0, bool discardWholeBuffer = false) = 0;

		/**
		 * @brief	Returns true if the texture can be bound to a shader.
		 *
		 * @note	This is only false for some rare special cases. (e.g. AA render texture in DX9)
		 *			Internal method.
		 */
		virtual bool isBindableAsShaderResource() const { return true; }

		/**
		 * @brief	Returns properties that contain information about the texture.
		 */
		const TextureProperties& getProperties() const { return mProperties; }

		/************************************************************************/
		/* 								TEXTURE VIEW                      		*/
		/************************************************************************/

		/**
		 * @brief	Requests a texture view for the specified mip and array ranges. Returns an existing view of one for
		 *			the specified ranges already exists, otherwise creates a new one. You must release all views
		 *			by calling "releaseView" when done.
		 *
		 * @note	Core thread only.
		 */
		static TextureViewPtr requestView(const SPtr<TextureCore>& texture, UINT32 mostDetailMip, UINT32 numMips,
			UINT32 firstArraySlice, UINT32 numArraySlices, GpuViewUsage usage);

		/**
		 * @brief	Releases the view. View won't actually get destroyed until all references to it are released.
		 *
		 * @note	Core thread only.
		 */
		static void releaseView(const TextureViewPtr& view);

	protected:
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
		virtual void copyImpl(UINT32 srcFace, UINT32 srcMipLevel, UINT32 destFace, UINT32 destMipLevel, const SPtr<TextureCore>& target) = 0;

		/************************************************************************/
		/* 								TEXTURE VIEW                      		*/
		/************************************************************************/

		/**
		 * @brief	Creates a new empty/undefined texture view.
		 */
		virtual TextureViewPtr createView(const SPtr<TextureCore>& texture, const TEXTURE_VIEW_DESC& desc);

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
		TextureProperties mProperties;
	};

	/**
	 * @brief	Abstract class representing a texture. Specific render systems have their
	 *			own Texture implementations. Internally represented as one or more surfaces
	 *			with pixels in a certain number of dimensions, backed by a hardware buffer.
	 *
	 * @note	Sim thread.
	 */
    class BS_CORE_EXPORT Texture : public Resource
    {
    public:
		/**
		 * @brief	Updates the texture with new data. The actual write will be queued for later execution on the core thread.
		 *			Provided data buffer will be locked until the operation completes.
		 *
		 * @param	accessor			Accessor to queue the operation on.
		 * 
		 * @return	Async operation object you can use to track operation completion.
		 *
		 * @see		TextureCore::writeSubresource
		 */
		AsyncOp writeSubresource(CoreAccessor& accessor, UINT32 subresourceIdx, const PixelDataPtr& data, bool discardEntireBuffer);

		/**
		 * @brief	Reads internal texture data to the provided previously allocated buffer. The read is queued for execution
		 *			on the core thread and not executed immediately. Provided data buffer will be locked until the
		 *			operation completes.
		 *
		 * @param	accessor			Accessor to queue the operation on.
		 *
		 * @return	Async operation object you can use to track operation completion.
		 *
		 * @see		TextureCore::readSubresource
		 */
		AsyncOp readSubresource(CoreAccessor& accessor, UINT32 subresourceIdx, const PixelDataPtr& data);

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
		 * @brief	Reads data from the cached system memory texture buffer into the provided buffer. 
		 * 		  
		 * @param	dest		Previously allocated buffer to read data into.
		 * @param	mipLevel	(optional) Mipmap level to read from.
		 * @param	face		(optional) Texture face to read from.
		 *
		 * @note	The data read is the cached texture data. Any data written to the texture from the GPU 
		 *			or core thread will not be reflected in this data. Use "readSubresource" if you require
		 *			those changes.
		 *
		 *			The texture must have been created with TU_CPUCACHED usage otherwise this method
		 *			will not return any data.
		 */
		void readData(PixelData& dest, UINT32 mipLevel = 0, UINT32 face = 0);

		/**
		 * @brief	Returns properties that contain information about the texture.
		 */
		const TextureProperties& getProperties() const { return mProperties; }

		/**
		 * @brief	Retrieves a core implementation of a texture usable only from the
		 *			core thread.
		 */
		SPtr<TextureCore> getCore() const;

		/**
		 * @brief	Returns a dummy 2x2 texture. Don't modify the returned texture.
		 * 			
		 * @note	Thread safe.
		 */
		static const HTexture& dummy();

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
		 */
		static HTexture create(TextureType texType, UINT32 width, UINT32 height, UINT32 depth,
			int numMips, PixelFormat format, int usage = TU_DEFAULT,
			bool hwGammaCorrection = false, UINT32 multisampleCount = 0);


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
		 */
		static HTexture create(TextureType texType, UINT32 width, UINT32 height, int numMips,
			PixelFormat format, int usage = TU_DEFAULT,
			bool hwGammaCorrection = false, UINT32 multisampleCount = 0);

		/**
		 * @copydoc	create(TextureType, UINT32, UINT32, UINT32, int, PixelFormat, int, bool, UINT32)
		 *
		 * @note	Internal method. Creates a texture pointer without a handle. Use "create" for normal usage.
		 */
		static TexturePtr _createPtr(TextureType texType, UINT32 width, UINT32 height, UINT32 depth,
			int num_mips, PixelFormat format, int usage = TU_DEFAULT,
			bool hwGammaCorrection = false, UINT32 multisampleCount = 0);

		/**
		 * @copydoc	create(TextureType, UINT32, UINT32, int, PixelFormat, int, bool, UINT32)
		 *
		 * @note	Internal method. Creates a texture pointer without a handle. Use "create" for normal usage.
		 */
		static TexturePtr _createPtr(TextureType texType, UINT32 width, UINT32 height, int num_mips,
			PixelFormat format, int usage = TU_DEFAULT, bool hwGammaCorrection = false, UINT32 multisampleCount = 0);

    protected:
		friend class TextureManager;

		Texture(TextureType textureType, UINT32 width, UINT32 height, UINT32 depth, UINT32 numMipmaps,
			PixelFormat format, int usage, bool hwGamma, UINT32 multisampleCount);

		/**
		 * @copydoc	Resource::initialize
		 */
		void initialize();

		/**
		 * @copydoc	CoreObject::createCore
		 */
		SPtr<CoreObjectCore> createCore() const;

		/**
		 * @copydoc	Resource::calculateSize
		 */
		UINT32 calculateSize() const;

		/**
		 * @brief	Creates buffers used for caching of CPU texture data.
		 *
		 * @note	Make sure to initialize all texture properties before calling this.
		 */
		void createCPUBuffers();

		/**
		 * @brief	Updates the cached CPU buffers with new data.
		 */
		void updateCPUBuffers(UINT32 subresourceIdx, const PixelData& data);

	protected:
		Vector<PixelDataPtr> mCPUSubresourceData;
		TextureProperties mProperties;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		Texture(); // Serialization only

		friend class TextureRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
    };
}