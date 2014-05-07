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
#pragma once

#include "CmPrerequisites.h"
#include "CmGpuResource.h"
#include "CmHardwareBuffer.h"
#include "CmPixelUtil.h"
#include "CmTextureView.h"

namespace BansheeEngine 
{
	/** Enum identifying the texture usage
    */
    enum TextureUsage
    {
		/// @copydoc HardwareBuffer::Usage
		TU_STATIC = GBU_STATIC, // Optimal setting if texture is read by the GPU often, and very rarely written by CPU
		TU_DYNAMIC = GBU_DYNAMIC, // Optimal if the texture is updated by CPU often (e.g. every frame)
		TU_RENDERTARGET = 0x200, // Used for rendering by the GPU
		TU_DEPTHSTENCIL = 0x400,
		TU_DEFAULT = TU_STATIC
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
    class CM_EXPORT Texture : public GpuResource
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
		 * @brief	Get the level of multisample AA to be used.
		 *
		 * @note	Thread safe.
		 */
		virtual UINT32 getFSAA() const { return mFSAA; }

		/**
		 * @brief	Get the multisample AA hint.
		 *
		 * @note	Thread safe.
		 */
		virtual const String& getFSAAHint() const { return mFSAAHint; }

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
		 * @brief	Returns true if the texture can be bound to a shader.
		 * 			
		 * @note	This is only false for some rare special cases. (e.g. AA render texture in DX9)
		 * 			
		 *			Core thread only.
		 */
		virtual bool isBindableAsShaderResource() const { return true; }

		/**
		 * @copydoc GpuResource::writeSubresource
		 */
		virtual void writeSubresource(UINT32 subresourceIdx, const GpuResourceData& data, bool discardEntireBuffer);

		/**
		 * @copydoc GpuResource::readSubresource
		 */
		virtual void readSubresource(UINT32 subresourceIdx, GpuResourceData& data);

		/**
		 * @brief	Allocates a buffer you may use for storage when reading a subresource. You
		 * 			need to allocate such a buffer if you are calling "readSubresource".
		 * 			
		 * @note	Thread safe.
		 */
		PixelDataPtr allocateSubresourceBuffer(UINT32 subresourceIdx) const;

		/**
		 * @brief	Maps a subresource index to an exact face and mip level. Subresource indexes
		 * 			are used when reading or writing to the resource.
		 * 			
		 * @note	Subresource index is only valid for the instance it was created on. You cannot use a subresource
		 * 			index from a different texture and expect to get valid result. Modifying the resource so the number
		 * 			of subresources changes, invalidates all subresource indexes.
		 * 			
		 *			Thread safe.
		 */
		void mapFromSubresourceIdx(UINT32 subresourceIdx, UINT32& face, UINT32& mip) const;

		/**
		 * @brief	Map a face and a mip level to a subresource index you can use for updating or reading
		 * 			a specific sub-resource.
		 * 			
		 * @note	Generated subresource index is only valid for the instance it was created on. Modifying the resource so the number
		 * 			of subresources changes, invalidates all subresource indexes.
		 * 			
		 *			Thread safe.
		 */
		UINT32 mapToSubresourceIdx(UINT32 face, UINT32 mip) const;

		/**
		 * @brief	Locks the buffer for reading or writing.
		 *
		 * @param	options 	Options for controlling the operation.
		 * @param	mipLevel	(optional) the mip level.
		 * @param	face		(optional) the face.
		 * 						
		 * @return	Pointer to the buffer data. Only valid until you call unlock.
		 * 			
		 * @note	If you are just reading or writing one block of data use
		 * 			readData/writeData methods as they can be must faster in certain situations.
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
		 * @note	Core thread only.
		 */
		virtual void readData(PixelData& dest, UINT32 mipLevel = 0, UINT32 face = 0) = 0;

		/**
		 * @brief	Writes data from the provided buffer into the texture buffer.
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

		/************************************************************************/
		/* 								TEXTURE VIEW                      		*/
		/************************************************************************/

		static TextureViewPtr requestView(TexturePtr texture, UINT32 mostDetailMip, UINT32 numMips, UINT32 firstArraySlice, UINT32 numArraySlices, GpuViewUsage usage);
		static void releaseView(TextureViewPtr view);

	protected:

		virtual TextureViewPtr createView();
		void clearBufferViews();

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

        UINT32 mHeight; // Immutable
        UINT32 mWidth; // Immutable
        UINT32 mDepth; // Immutable

		UINT32 mNumMipmaps; // Immutable
		bool mHwGamma; // Immutable
		UINT32 mFSAA; // Immutable
		String mFSAAHint; // Immutable

        TextureType mTextureType; // Immutable
		PixelFormat mFormat; // Immutable
        int mUsage; // Immutable

		Texture();

		/**
		 * @brief	Initializes the texture. This must be called right after the texture is constructed. Called by TextureManager
		 * 			upon texture creation, so usually you don't want to call this manually.
		 * 			
		 * @note	Initialization is not done immediately, and is instead just scheduled on the
		 * 			core thread. Unless called from core thread, in which case it is initialized
		 * 			right away.
		 */
		void initialize(TextureType textureType, UINT32 width, UINT32 height, UINT32 depth, UINT32 numMipmaps, 
			PixelFormat format, int usage, bool hwGamma, UINT32 fsaa, const String& fsaaHint);

		virtual PixelData lockImpl(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0) = 0;
		virtual void unlockImpl() = 0;

		virtual void copyImpl(TexturePtr& target) = 0;
		/// @copydoc Resource::calculateSize
		UINT32 calculateSize(void) const;

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
		static HTexture create(TextureType texType, UINT32 width, UINT32 height, UINT32 depth, 
			int num_mips, PixelFormat format, int usage = TU_DEFAULT,
			bool hwGammaCorrection = false, UINT32 fsaa = 0, const String& fsaaHint = StringUtil::BLANK);

		static HTexture create(TextureType texType, UINT32 width, UINT32 height, int num_mips,
			PixelFormat format, int usage = TU_DEFAULT,
			bool hwGammaCorrection = false, UINT32 fsaa = 0, const String& fsaaHint = StringUtil::BLANK);

		static TexturePtr _createPtr(TextureType texType, UINT32 width, UINT32 height, UINT32 depth, 
			int num_mips, PixelFormat format, int usage = TU_DEFAULT,
			bool hwGammaCorrection = false, UINT32 fsaa = 0, const String& fsaaHint = StringUtil::BLANK);

		static TexturePtr _createPtr(TextureType texType, UINT32 width, UINT32 height, int num_mips,
			PixelFormat format, int usage = TU_DEFAULT,
			bool hwGammaCorrection = false, UINT32 fsaa = 0, const String& fsaaHint = StringUtil::BLANK);
    };
}