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
#ifndef _TextureManager_H__
#define _TextureManager_H__

#include "CmPrerequisites.h"

#include "CmTexture.h"
#include "CmModule.h"


namespace CamelotEngine {

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Resources
	*  @{
	*/
	/** Class for loading & managing textures.
        @remarks
            Note that this class is abstract - the particular
            RenderSystem that is in use at the time will create
            a concrete subclass of this. Note that the concrete
            class will be available via the abstract singleton
            obtained from TextureManager::getSingleton(), but
            you should not assume that it is available until you
            have a) initialised Ogre (after selecting a RenderSystem
            and calling initialise from the Root object), and b)
            created at least one window - this may be done at the
            same time as part a if you allow Ogre to autocreate one.
     */
    class CM_EXPORT TextureManager : public Module<TextureManager>
    {
	protected:
		virtual Texture* createTextureImpl() = 0;
		virtual RenderTexture* createRenderTextureImpl() = 0;

		void destroy(Texture* texture);
		virtual void destroy_internal(Texture* texture);

    public:

        TextureManager(void);
        virtual ~TextureManager();

		/** Create a manual texture with specified width, height and depth (not loaded from a file).
            @param
                name The name to give the resulting texture
            @param
                group The name of the resource group to assign the texture to
            @param
                texType The type of texture to load/create, defaults to normal 2D textures
            @param
                width, height, depth The dimensions of the texture
            @param
                numMipmaps The number of pre-filtered mipmaps to generate. If left to MIP_DEFAULT then
                the TextureManager's default number of mipmaps will be used (see setDefaultNumMipmaps())
				If set to MIP_UNLIMITED mipmaps will be generated until the lowest possible
				level, 1x1x1.
            @param
                format The internal format you wish to request; the manager reserves
                the right to create a different format if the one you select is
                not available in this context.
			@param 
				usage The kind of usage this texture is intended for. It 
				is a combination of TU_STATIC, TU_DYNAMIC, TU_WRITE_ONLY, 
				TU_AUTOMIPMAP and TU_RENDERTARGET (see TextureUsage enum). You are
            	strongly advised to use HBU_STATIC_WRITE_ONLY wherever possible, if you need to 
            	update regularly, consider HBU_DYNAMIC_WRITE_ONLY.
            @param
                loader If you intend the contents of the manual texture to be 
                regularly updated, to the extent that you don't need to recover 
                the contents if the texture content is lost somehow, you can leave
                this parameter as 0. However, if you intend to populate the
                texture only once, then you should implement ManualResourceLoader
                and pass a pointer to it in this parameter; this means that if the
                manual texture ever needs to be reloaded, the ManualResourceLoader
                will be called to do it.
			@param hwGammaCorrection Pass 'true' to enable hardware gamma correction
				(sRGB) on this texture. The hardware will convert from gamma space
				to linear space when reading from this texture. Only applicable for 
				8-bits per channel textures, will be ignored for other types. Has the advantage
				over pre-applied gamma that the texture precision is maintained.
			@param fsaa The level of multisampling to use if this is a render target. Ignored
				if usage does not include TU_RENDERTARGET or if the device does
				not support it.
        */
        TexturePtr createTexture(TextureType texType, UINT32 width, UINT32 height, UINT32 depth, 
			int num_mips, PixelFormat format, int usage = TU_DEFAULT, bool hwGammaCorrection = false, 
			UINT32 fsaa = 0, const String& fsaaHint = StringUtil::BLANK);
			
        /** Create a manual texture with a depth of 1 (not loaded from a file).
            @param
                name The name to give the resulting texture
            @param
                group The name of the resource group to assign the texture to
            @param
                texType The type of texture to load/create, defaults to normal 2D textures
            @param
                width, height The dimensions of the texture
            @param
                numMipmaps The number of pre-filtered mipmaps to generate. If left to MIP_DEFAULT then
                the TextureManager's default number of mipmaps will be used (see setDefaultNumMipmaps()).
				If set to MIP_UNLIMITED mipmaps will be generated until the lowest possible
				level, 1x1x1.
            @param
                format The internal format you wish to request; the manager reserves
                the right to create a different format if the one you select is
                not available in this context.
			@param 
				usage The kind of usage this texture is intended for. It 
				is a combination of TU_STATIC, TU_DYNAMIC, TU_WRITE_ONLY, 
				TU_AUTOMIPMAP and TU_RENDERTARGET (see TextureUsage enum). You are
            	strongly advised to use HBU_STATIC_WRITE_ONLY wherever possible, if you need to 
            	update regularly, consider HBU_DYNAMIC_WRITE_ONLY.
            @param
                loader If you intend the contents of the manual texture to be 
                regularly updated, to the extent that you don't need to recover 
                the contents if the texture content is lost somehow, you can leave
                this parameter as 0. However, if you intend to populate the
                texture only once, then you should implement ManualResourceLoader
                and pass a pointer to it in this parameter; this means that if the
                manual texture ever needs to be reloaded, the ManualResourceLoader
                will be called to do it.
			 @param hwGammaCorrection Pass 'true' to enable hardware gamma correction
				 (sRGB) on this texture. The hardware will convert from gamma space
				 to linear space when reading from this texture. Only applicable for 
				 8-bits per channel textures, will be ignored for other types. Has the advantage
				 over pre-applied gamma that the texture precision is maintained.
			@param fsaa The level of multisampling to use if this is a render target. Ignored
				if usage does not include TU_RENDERTARGET or if the device does
				not support it.
        */
        TexturePtr createTexture(TextureType texType, UINT32 width, UINT32 height, int num_mips,
            PixelFormat format, int usage = TU_DEFAULT, bool hwGammaCorrection = false, UINT32 fsaa = 0, 
			const String& fsaaHint = StringUtil::BLANK)
		{
			return createTexture(texType, width, height, 1, 
				num_mips, format, usage, hwGammaCorrection, fsaa, fsaaHint);
		}

		/**
		 * @brief	Creates a completely empty and uninitialized Texture.
		 * 			Should only be used for VERY specific purposes, like deserialization,
		 * 			as it requires additional manual initialization that is not required normally.
		 */
		TexturePtr createEmpty();

		/**
		 * @brief	Creates a new RenderTexture and automatically generates a color surface
		 * 			and (optionally) a depth/stencil surface
		 */
		virtual RenderTexturePtr createRenderTexture(TextureType textureType, UINT32 width, UINT32 height, 
			PixelFormat format, bool hwGamma, UINT32 fsaa, const String& fsaaHint, 
			bool createDepth = true, PixelFormat depthStencilFormat = PF_D24S8);

		/**
		 * @brief	Creates an empty RenderTexture. You need to assign color and depth/stencil buffers to
		 * 			it manually before using it.
		 */
		virtual RenderTexturePtr createEmptyRenderTexture();

		/**
		 * @brief	Creates a new multi render texture. You may use this type of texture
		 * 			to render to multiple output textures at once.
		 */
		virtual MultiRenderTexturePtr createMultiRenderTexture() = 0;

		/** Returns whether this render system can natively support the precise texture 
			format requested with the given usage options.
		@remarks
			You can still create textures with this format even if this method returns
			false; the texture format will just be altered to one which the device does
			support.
		@note
			Sometimes the device may just slightly change the format, such as reordering the 
			channels or packing the channels differently, without it making and qualitative 
			differences to the texture. If you want to just detect whether the quality of a
			given texture will be reduced, use isEquivalentFormatSupport instead.
		@param format The pixel format requested
		@param usage The kind of usage this texture is intended for, a combination of 
			the TextureUsage flags.
		@returns true if the format is natively supported, false if a fallback would be used.
		*/
		virtual bool isFormatSupported(TextureType ttype, PixelFormat format, int usage);

		/** Returns whether this render system can support the texture format requested
			with the given usage options, or another format with no quality reduction.
		*/
		virtual bool isEquivalentFormatSupported(TextureType ttype, PixelFormat format, int usage);

		/** Gets the format which will be natively used for a requested format given the
			constraints of the current device.
		*/
		virtual PixelFormat getNativeFormat(TextureType ttype, PixelFormat format, int usage) = 0;

        /** Returns whether this render system has hardware filtering supported for the
            texture format requested with the given usage options.
        @remarks
            Not all texture format are supports filtering by the hardware, i.e. some
            cards support floating point format, but it doesn't supports filtering on
            the floating point texture at all, or only a subset floating point formats
            have flitering supported.
        @par
            In the case you want to write shader to work with floating point texture, and
            you want to produce better visual quality, it's necessary to flitering the
            texture manually in shader (potential requires four or more texture fetch
            instructions, plus several arithmetic instructions) if filtering doesn't
            supported by hardware. But in case on the hardware that supports floating
            point filtering natively, it had better to adopt this capability for
            performance (because only one texture fetch instruction are required) and
            doesn't loss visual quality.
        @par
            This method allow you queries hardware texture filtering capability to deciding
            which verion of the shader to be used. Note it's up to you to write multi-version
            shaders for support various hardware, internal engine can't do that for you
            automatically.
        @note
            Under GL, texture filtering are always supported by driver, but if it's not
            supported by hardware natively, software simulation will be used, and you
            will end up with very slow speed (less than 0.1 fps for example). To slove
            this performance problem, you must disable filtering manually (by use
            <b>filtering none</b> in the material script's texture_unit section, or
            call TextureUnitState::setTextureFiltering with TFO_NONE if populate
            material in code).
		@param ttype The texture type requested
		@param format The pixel format requested
		@param usage The kind of usage this texture is intended for, a combination of 
			the TextureUsage flags.
        @param preciseFormatOnly Whether precise or fallback format mode is used to detecting.
            In case the pixel format doesn't supported by device, false will be returned
            if in precise mode, and natively used pixel format will be actually use to
            check if in fallback mode.
		@returns true if the texture filtering is supported.
        */
        virtual bool isHardwareFilteringSupported(TextureType ttype, PixelFormat format, int usage,
            bool preciseFormatOnly = false) = 0;

    protected:

    };
	/** @} */
	/** @} */
}// Namespace

#endif
