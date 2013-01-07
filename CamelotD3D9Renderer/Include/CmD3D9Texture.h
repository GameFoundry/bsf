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
#ifndef __D3D9TEXTURE_H__
#define __D3D9TEXTURE_H__

#include "CmD3D9Prerequisites.h"
#include "CmTexture.h"
#include "CmRenderTexture.h"
#include "CmException.h"
#include "CmD3D9HardwarePixelBuffer.h"
#include "CmD3D9Resource.h"

namespace CamelotEngine {
	class CM_D3D9_EXPORT D3D9Texture : public Texture, public D3D9Resource
	{
		public:
		/// destructor
		~D3D9Texture();

		/// retrieves a pointer to the actual texture
		IDirect3DBaseTexture9 *getTexture_internal();		
		/// retrieves a pointer to the normal 1D/2D texture
		IDirect3DTexture9 *getNormTexture_internal();
		/// retrieves a pointer to the cube texture
		IDirect3DCubeTexture9 *getCubeTexture_internal();

		/** Indicates whether the hardware gamma is actually enabled and supported. 
		@remarks
			Because hardware gamma might not actually be supported, we need to 
			ignore it sometimes. Because D3D doesn't encode sRGB in the format but
			as a sampler state, and we don't want to change the original requested
			hardware gamma flag (e.g. serialisation) we need another indicator.
		*/
		bool isHardwareGammaReadToBeUsed() const { return mHwGamma && mHwGammaReadSupported; }
					
		/// Will this texture need to be in the default pool?
		bool useDefaultPool();

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
		HardwarePixelBufferPtr getBuffer(UINT32 face, UINT32 mipmap);

		// Called immediately after the Direct3D device has been created.
		virtual void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device);

		// Called before the Direct3D device is going to be destroyed.
		virtual void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device);

		// Called immediately after the Direct3D device has entered a lost state.
		virtual void notifyOnDeviceLost(IDirect3DDevice9* d3d9Device);

		// Called immediately after the Direct3D device has been reset
		virtual void notifyOnDeviceReset(IDirect3DDevice9* d3d9Device);	

	protected:	
		friend class D3D9TextureManager;
		friend class D3D9HardwarePixelBuffer;

		struct TextureResources
		{
			/// 1D/2D normal texture pointer
			IDirect3DTexture9* pNormTex;	
			/// cubic texture pointer
			IDirect3DCubeTexture9* pCubeTex;	
			/// Volume texture
			IDirect3DVolumeTexture9* pVolumeTex;
			/// actual texture pointer
			IDirect3DBaseTexture9* pBaseTex;
			/// Optional FSAA surface
			IDirect3DSurface9* pFSAASurface;			
		};
		
		typedef map<IDirect3DDevice9*, TextureResources*>::type	DeviceToTextureResourcesMap;
		typedef DeviceToTextureResourcesMap::iterator			DeviceToTextureResourcesIterator;

		/// Map between device to texture resources.
		DeviceToTextureResourcesMap	mMapDeviceToTextureResources;

		/// Vector of pointers to subsurfaces
		typedef vector<HardwarePixelBufferPtr>::type SurfaceList;
		SurfaceList	mSurfaceList;
		/// cube texture individual face names
		String							mCubeFaceNames[6];	
		/// The memory pool being used
		D3DPOOL							mD3DPool;
		// Dynamic textures?
		bool                            mDynamicTextures;
		
		HardwarePixelBufferPtr			mLockedBuffer;

		/// Is hardware gamma supported (read)?
		bool mHwGammaReadSupported;
		/// Is hardware gamma supported (write)?
		bool mHwGammaWriteSupported;
		D3DMULTISAMPLE_TYPE mFSAAType;
		DWORD mFSAAQuality;

		D3D9Texture();

		/// overriden from Resource
		void initialize_internal();	

		/// overridden from Texture
		void copyImpl(TexturePtr& target);

		PixelData lockImpl(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0);
		void unlockImpl();

		/// internal method, create a blank normal 1D/2D texture		
		void _createNormTex(IDirect3DDevice9* d3d9Device);
		/// internal method, create a blank cube texture		
		void _createCubeTex(IDirect3DDevice9* d3d9Device);
		/// internal method, create a blank cube texture		
		void _createVolumeTex(IDirect3DDevice9* d3d9Device);

		/// internal method, return a D3D pixel format for texture creation
		D3DFORMAT _chooseD3DFormat(IDirect3DDevice9* d3d9Device);

		/// @copydoc Resource::calculateSize
		UINT32 calculateSize(void) const;
		/// @copydoc Texture::createInternalResourcesImpl
		void createInternalResourcesImpl(void);
		/// Creates this texture resources on the specified device.
		void createInternalResourcesImpl(IDirect3DDevice9* d3d9Device);
		/// free internal resources
		void freeInternalResourcesImpl(void);
		/// internal method, set Texture class final texture protected attributes
		void _setFinalAttributes(IDirect3DDevice9* d3d9Device, TextureResources* textureResources, 
			unsigned long width, unsigned long height, unsigned long depth, PixelFormat format);
		/// internal method, return the best by hardware supported filter method
		D3DTEXTUREFILTERTYPE _getBestFilterMethod(IDirect3DDevice9* d3d9Device);
		/// internal method, return true if the device/texture combination can use dynamic textures
		bool _canUseDynamicTextures(IDirect3DDevice9* d3d9Device, DWORD srcUsage, D3DRESOURCETYPE srcType, D3DFORMAT srcFormat);
		/// internal method, return true if the device/texture combination can auto gen. mip maps
		bool _canAutoGenMipmaps(IDirect3DDevice9* d3d9Device, DWORD srcUsage, D3DRESOURCETYPE srcType, D3DFORMAT srcFormat);
		/// internal method, return true if the device/texture combination can use hardware gamma
		bool _canUseHardwareGammaCorrection(IDirect3DDevice9* d3d9Device, DWORD srcUsage, D3DRESOURCETYPE srcType, D3DFORMAT srcFormat, bool forwriting);
		
		/// internal method, the cube map face name for the spec. face index
		String _getCubeFaceName(unsigned char face) const
		{ assert(face < 6); return mCubeFaceNames[face]; }

		/// internal method, create D3D9HardwarePixelBuffers for every face and
		/// mipmap level. This method must be called after the D3D texture object was created
		void _createSurfaceList(IDirect3DDevice9* d3d9Device, TextureResources* textureResources);
	 
		/// gets the texture resources attached to the given device.
		TextureResources* getTextureResources(IDirect3DDevice9* d3d9Device);

		/// allocates new texture resources structure attached to the given device.
		TextureResources* allocateTextureResources(IDirect3DDevice9* d3d9Device);

		/// creates this texture resources according to the current settings.
		void createTextureResources(IDirect3DDevice9* d3d9Device);

		/// frees the given texture resources.
		void freeTextureResources(IDirect3DDevice9* d3d9Device, TextureResources* textureResources);

		void determinePool();
    };
}

#endif
