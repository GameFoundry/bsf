/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef _Texture_H__
#define _Texture_H__

#include "CmPrerequisites.h"
#include "CmResource.h"
#include "CmHardwareBuffer.h"
#include "CmPixelUtil.h"

namespace CamelotEngine {

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Resources
	*  @{
	*/
	/** Enum identifying the texture usage
    */
    enum TextureUsage
    {
		/// @copydoc HardwareBuffer::Usage
		TU_STATIC = HardwareBuffer::HBU_STATIC,
		TU_DYNAMIC = HardwareBuffer::HBU_DYNAMIC,
		TU_WRITE_ONLY = HardwareBuffer::HBU_WRITE_ONLY,
		TU_STATIC_WRITE_ONLY = HardwareBuffer::HBU_STATIC_WRITE_ONLY, 
		TU_DYNAMIC_WRITE_ONLY = HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY,
		TU_DYNAMIC_WRITE_ONLY_DISCARDABLE = HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE,
		/// this texture will be a render target, i.e. used as a target for render to texture
		/// setting this flag will ignore all other texture usages
		TU_RENDERTARGET = 0x200,
		/// default to automatic mipmap generation static textures
		TU_DEFAULT = TU_STATIC_WRITE_ONLY
        
    };

    /** Enum identifying the texture type
    */
    enum TextureType
    {
        /// 1D texture, used in combination with 1D texture coordinates
        TEX_TYPE_1D = 1,
        /// 2D texture, used in combination with 2D texture coordinates (default)
        TEX_TYPE_2D = 2,
        /// 3D volume texture, used in combination with 3D texture coordinates
        TEX_TYPE_3D = 3,
        /// 3D cube map, used in combination with 3D texture coordinates
        TEX_TYPE_CUBE_MAP = 4
    };

	/** Enum identifying special mipmap numbers
    */
	enum TextureMipmap
	{
		/// Generate mipmaps up to 1x1
		MIP_UNLIMITED = 0x7FFFFFFF
	};

    /** Abstract class representing a Texture resource.
        @remarks
            The actual concrete subclass which will exist for a texture
            is dependent on the rendering system in use (Direct3D, OpenGL etc).
            This class represents the commonalities, and is the one 'used'
            by programmers even though the real implementation could be
            different in reality. Texture objects are created through
            the 'create' method of the TextureManager concrete subclass.
     */
    class CM_EXPORT Texture : public Resource
    {
    public:
        /** Gets the type of texture 
        */
        virtual TextureType getTextureType(void) const { return mTextureType; }

        /** Gets the number of mipmaps to be used for this texture.
        */
        virtual size_t getNumMipmaps(void) const {return mNumMipmaps;}

		/** Gets whether this texture will be set up so that on sampling it, 
		hardware gamma correction is applied.
		*/
		virtual bool isHardwareGammaEnabled() const { return mHwGamma; }

		/** Get the level of multisample AA to be used if this texture is a 
		rendertarget.
		*/
		virtual UINT32 getFSAA() const { return mFSAA; }

		/** Get the multisample AA hint if this texture is a rendertarget.
		*/
		virtual const String& getFSAAHint() const { return mFSAAHint; }

		/** Returns the height of the texture.
        */
        virtual size_t getHeight(void) const { return mHeight; }

        /** Returns the width of the texture.
        */
        virtual size_t getWidth(void) const { return mWidth; }

        /** Returns the depth of the texture (only applicable for 3D textures).
        */
        virtual size_t getDepth(void) const { return mDepth; }

        /** Returns the TextureUsage indentifier for this Texture
        */
        virtual int getUsage() const { return mUsage; }

		/** Returns the pixel format for the texture surface. */
		virtual PixelFormat getFormat() const { return mFormat; }

        /** Returns true if the texture has an alpha layer. */
        virtual bool hasAlpha(void) const;

        /** Return the number of faces this texture has. This will be 6 for a cubemap
        	texture and 1 for a 1D, 2D or 3D one.
        */
        virtual size_t getNumFaces() const;

		/** Return hardware pixel buffer for a surface. This buffer can then
			be used to copy data from and to a particular level of the texture.
			@param face 	Face number, in case of a cubemap texture. Must be 0
							for other types of textures.
                            For cubemaps, this is one of 
                            +X (0), -X (1), +Y (2), -Y (3), +Z (4), -Z (5)
			@param mipmap	Mipmap level. This goes from 0 for the first, largest
							mipmap level to getNumMipmaps()-1 for the smallest.
			@returns	A shared pointer to a hardware pixel buffer
			@remarks	The buffer is invalidated when the resource is unloaded or destroyed.
						Do not use it after the lifetime of the containing texture.
		*/
		virtual HardwarePixelBufferPtr getBuffer_internal(size_t face=0, size_t mipmap=0) = 0;
		
		/** Retrieve a platform or API-specific piece of information from this texture.
		 This method of retrieving information should only be used if you know what you're doing.
		 @param name The name of the attribute to retrieve
		 @param pData Pointer to memory matching the type of data you want to retrieve.
		*/
		virtual void getCustomAttribute_internal(const String& name, void* pData);

		/**
		 * @brief	Sets raw texture pixels for the specified mip level and texture face. Pixel format
		 * 			must match the format of the texture.
		 * 			
		 * @note	Not-async. This operation will block the current thread until the render thread
		 *			executes the command.
		 */
		void setRawPixels(const PixelData& data, UINT32 face = 0, UINT32 mip = 0);

		/**
		 * @brief	Sets raw texture pixels for the specified mip level and texture face. Pixel format
		 * 			must match the format of the texture. Returns immediately
		 * 			but the texture won't be updated until the command
		 * 			executes on the render thread.
		 * 			
		 * @see		Texture::setRawPixels		
		 */
		void setRawPixels_async(const PixelData& data, UINT32 face = 0, UINT32 mip = 0);

		/**
		 * @brief	Internal version of Texture::setRawPixels. Only callable
		 * 			from the render thread.
		 *
		 * @see		Texture::setRawPixels
		 */
		void setRawPixels_internal(const PixelData& data, UINT32 face = 0, UINT32 mip = 0);

		/**
		 * @brief	Gets raw pixels from the texture. This is a slow operation
		 * 			as it will read data from the GPU. If the texture is compressed
		 * 			the returned data will be contain compressed pixels as well.
		 * 			
		 * @note	Not-async. This operation will block the current thread until the render thread
		 *			executes the command.
		 */
		PixelDataPtr getRawPixels(UINT32 face = 0, UINT32 mip = 0);
		

		/**
		 * @brief	Async version of Texture::getRawPixels. Returns immediately
		 * 			but you won't have access to the pixel data until the command
		 * 			executes on the render thread.
		 *
		 * @see		Texture::getRawPixels
		 */
		AsyncOp getRawPixels_async(UINT32 face = 0, UINT32 mip = 0);

		/**
		 * @brief	Internal version of Texture::getRawPixels. Only callable
		 * 			from the render thread.
		 *
		 * @see		Texture::getRawPixels
		 */
		void getRawPixels_internal(UINT32 face, UINT32 mip, AsyncOp& op);

		/** Copies (and maybe scales to fit) the contents of this texture to
			another texture. */
		virtual void copy_internal( TexturePtr& target );

    protected:
		friend class TextureManager;

        size_t mHeight;
        size_t mWidth;
        size_t mDepth;

		size_t mNumMipmaps;
		bool mHwGamma;
		UINT32 mFSAA;
		String mFSAAHint;

        TextureType mTextureType;
		PixelFormat mFormat;
        int mUsage; // Bit field, so this can't be TextureUsage

		Texture();

		/**
		 * @brief	Initializes the texture. This must be called right after the texture is constructed. Called by TextureManager
		 * 			upon texture creation, so usually you don't want to call this manually.
		 * 			
		 * @note	Initialization is not done immediately, and is instead just scheduled on the render thread.
		 */
		void initialize(TextureType textureType, size_t width, size_t height, size_t depth, size_t numMipmaps, 
			PixelFormat format, int usage, bool hwGamma, UINT32 fsaa, const String& fsaaHint);
		
		/**
		 * @brief	Performs GpuProgram initialization. Only callable from the render thread.
		 */
		virtual void initialize_internal() = 0;

		/// @copydoc Resource::calculateSize
		size_t calculateSize(void) const;

        /** Creates the internal texture resources for this texture. 
        @remarks
            This method creates the internal texture resources (pixel buffers, 
            texture surfaces etc) required to begin using this texture. You do
            not need to call this method directly unless you are manually creating
            a texture, in which case something must call it, after having set the
            size and format of the texture (e.g. the ManualResourceLoader might
            be the best one to call it). If you are not defining a manual texture,
            or if you use one of the self-contained load...() methods, then it will be
            called for you.
        */
        virtual void createInternalResources(void);

        /** Frees internal texture resources for this texture. 
        */
        virtual void freeInternalResources(void);

		/** Implementation of creating internal texture resources 
		*/
		virtual void createInternalResourcesImpl(void) = 0;

		/** Implementation of freeing internal texture resources 
		*/
		virtual void freeInternalResourcesImpl(void) = 0;

		/** Default implementation of unload which calls freeInternalResources */
		void unloadImpl(void);

		void throwIfNotRenderThread() const;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class TextureRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

		/************************************************************************/
		/* 								STATICS		                     		*/
		/************************************************************************/
	public:
		static TexturePtr create(TextureType texType, UINT32 width, UINT32 height, UINT32 depth, 
			int num_mips, PixelFormat format, int usage = TU_DEFAULT,
			bool hwGammaCorrection = false, UINT32 fsaa = 0, const String& fsaaHint = StringUtil::BLANK);

		static TexturePtr create(TextureType texType, UINT32 width, UINT32 height, int num_mips,
			PixelFormat format, int usage = TU_DEFAULT,
			bool hwGammaCorrection = false, UINT32 fsaa = 0, const String& fsaaHint = StringUtil::BLANK);
    };

	/** @} */

}

#endif
