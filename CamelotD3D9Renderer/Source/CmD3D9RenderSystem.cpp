/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

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
#include "CmD3D9RenderSystem.h"
#include "CmD3D9Prerequisites.h"
#include "CmD3D9DriverList.h"
#include "CmD3D9Driver.h"
#include "CmD3D9VideoModeList.h"
#include "CmD3D9VideoMode.h"
#include "CmD3D9RenderWindow.h"
#include "CmD3D9TextureManager.h"
#include "CmD3D9Texture.h"
#include "CmMath.h"
#include "CmCamera.h"
#include "CmD3D9HardwareBufferManager.h"
#include "CmD3D9HardwareIndexBuffer.h"
#include "CmD3D9HardwareVertexBuffer.h"
#include "CmD3D9VertexDeclaration.h"
#include "CmD3D9GpuProgram.h"
#include "CmD3D9GpuProgramManager.h"
#include "CmD3D9HLSLProgramFactory.h"
#include "CmD3D9HardwareOcclusionQuery.h"
#include "CmD3D9MultiRenderTarget.h"
#include "CmD3D9DeviceManager.h"
#include "CmD3D9ResourceManager.h"
#include "CmHighLevelGpuProgramManager.h"
#include "CmAsyncOp.h"

#define FLOAT2DWORD(f) *((DWORD*)&f)

namespace CamelotEngine 
{
	D3D9RenderSystem* D3D9RenderSystem::msD3D9RenderSystem = NULL;

	//---------------------------------------------------------------------
	D3D9RenderSystem::D3D9RenderSystem( HINSTANCE hInstance )
	{
		// update singleton access pointer.
		msD3D9RenderSystem = this;

		// set the instance being passed 
		mhInstance = hInstance;

		// set pointers to NULL
		mpD3D = NULL;		
		mDriverList = NULL;
		mActiveD3DDriver = NULL;	
		mUseNVPerfHUD = false;
		mHLSLProgramFactory = NULL;		
		mCgProgramFactory = NULL;
		mDeviceManager = NULL;	
		mResourceManager = nullptr;		
	}
	//---------------------------------------------------------------------
	D3D9RenderSystem::~D3D9RenderSystem()
	{		
		shutdown();

		// Deleting the HLSL program factory
		if (mHLSLProgramFactory)
		{
			HighLevelGpuProgramManager::instance().removeFactory(mHLSLProgramFactory);
			delete mHLSLProgramFactory;
			mHLSLProgramFactory = 0;
		}

		if(mCgProgramFactory)
		{
			HighLevelGpuProgramManager::instance().removeFactory(mCgProgramFactory);
			delete mCgProgramFactory;
			mCgProgramFactory = 0;
		}
		
		SAFE_RELEASE( mpD3D );
		SAFE_DELETE ( mResourceManager );

		msD3D9RenderSystem = NULL;
	}
	//---------------------------------------------------------------------
	const String& D3D9RenderSystem::getName() const
	{
		static String strName( "D3D9RenderSystem");
		return strName;
	}
	//---------------------------------------------------------------------
	D3D9DriverList* D3D9RenderSystem::getDirect3DDrivers()
	{
		if( !mDriverList )
			mDriverList = new D3D9DriverList();

		return mDriverList;
	}
	//---------------------------------------------------------------------
	bool D3D9RenderSystem::_checkMultiSampleQuality(D3DMULTISAMPLE_TYPE type, DWORD *outQuality, D3DFORMAT format, UINT adapterNum, D3DDEVTYPE deviceType, BOOL fullScreen)
	{
		HRESULT hr;
		hr = mpD3D->CheckDeviceMultiSampleType( 
			adapterNum, 
			deviceType, 
			format, 
			fullScreen, 
			type, 
			outQuality);

		if (SUCCEEDED(hr))
			return true;
		else
			return false;
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::initConfigOptions()
	{
		D3D9DriverList* driverList;
		D3D9Driver* driver;

		ConfigOption optDevice;
		ConfigOption optVideoMode;
		ConfigOption optFullScreen;
		ConfigOption optVSync;
		ConfigOption optVSyncInterval;
		ConfigOption optAA;
		ConfigOption optFPUMode;
		ConfigOption optNVPerfHUD;
		ConfigOption optSRGB;
		ConfigOption optResourceCeationPolicy;

		driverList = this->getDirect3DDrivers();

		optDevice.name = "Rendering Device";
		optDevice.currentValue.clear();
		optDevice.possibleValues.clear();
		optDevice.immutable = false;

		optVideoMode.name = "Video Mode";
		optVideoMode.currentValue = "800 x 600 @ 32-bit colour";
		optVideoMode.immutable = false;

		optFullScreen.name = "Full Screen";
		optFullScreen.possibleValues.push_back( "Yes" );
		optFullScreen.possibleValues.push_back( "No" );
		optFullScreen.currentValue = "Yes";
		optFullScreen.immutable = false;

		optResourceCeationPolicy.name = "Resource Creation Policy";		
		optResourceCeationPolicy.possibleValues.push_back( "Create on all devices" );
		optResourceCeationPolicy.possibleValues.push_back( "Create on active device" );

		if (mResourceManager->getCreationPolicy() == RCP_CREATE_ON_ACTIVE_DEVICE)
			optResourceCeationPolicy.currentValue = "Create on active device";			
		else if (mResourceManager->getCreationPolicy() == RCP_CREATE_ON_ALL_DEVICES)
			optResourceCeationPolicy.currentValue = "Create on all devices";
		else
			optResourceCeationPolicy.currentValue = "N/A";
		optResourceCeationPolicy.immutable = false;

		for( unsigned j=0; j < driverList->count(); j++ )
		{
			driver = driverList->item(j);
			optDevice.possibleValues.push_back( driver->DriverDescription() );
			// Make first one default
			if( j==0 )
				optDevice.currentValue = driver->DriverDescription();
		}

		optVSync.name = "VSync";
		optVSync.immutable = false;
		optVSync.possibleValues.push_back( "Yes" );
		optVSync.possibleValues.push_back( "No" );
		optVSync.currentValue = "No";

		optVSyncInterval.name = "VSync Interval";
		optVSyncInterval.immutable = false;
		optVSyncInterval.possibleValues.push_back( "1" );
		optVSyncInterval.possibleValues.push_back( "2" );
		optVSyncInterval.possibleValues.push_back( "3" );
		optVSyncInterval.possibleValues.push_back( "4" );
		optVSyncInterval.currentValue = "1";

		optAA.name = "FSAA";
		optAA.immutable = false;
		optAA.possibleValues.push_back( "None" );
		optAA.currentValue = "None";

		optFPUMode.name = "Floating-point mode";
#if OGRE_DOUBLE_PRECISION
		optFPUMode.currentValue = "Consistent";
#else
		optFPUMode.currentValue = "Fastest";
#endif
		optFPUMode.possibleValues.clear();
		optFPUMode.possibleValues.push_back("Fastest");
		optFPUMode.possibleValues.push_back("Consistent");
		optFPUMode.immutable = false;

		optNVPerfHUD.currentValue = "No";
		optNVPerfHUD.immutable = false;
		optNVPerfHUD.name = "Allow NVPerfHUD";
		optNVPerfHUD.possibleValues.push_back( "Yes" );
		optNVPerfHUD.possibleValues.push_back( "No" );


		// SRGB on auto window
		optSRGB.name = "sRGB Gamma Conversion";
		optSRGB.possibleValues.push_back("Yes");
		optSRGB.possibleValues.push_back("No");
		optSRGB.currentValue = "No";
		optSRGB.immutable = false;

		mOptions[optDevice.name] = optDevice;
		mOptions[optVideoMode.name] = optVideoMode;
		mOptions[optFullScreen.name] = optFullScreen;
		mOptions[optVSync.name] = optVSync;
		mOptions[optVSyncInterval.name] = optVSyncInterval;
		mOptions[optAA.name] = optAA;
		mOptions[optFPUMode.name] = optFPUMode;
		mOptions[optNVPerfHUD.name] = optNVPerfHUD;
		mOptions[optSRGB.name] = optSRGB;
		mOptions[optResourceCeationPolicy.name] = optResourceCeationPolicy;

		refreshD3DSettings();

	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::refreshD3DSettings()
	{
		ConfigOption* optVideoMode;
		D3D9Driver* driver = 0;
		D3D9VideoMode* videoMode;

		ConfigOptionMap::iterator opt = mOptions.find( "Rendering Device" );
		if( opt != mOptions.end() )
		{
			for( unsigned j=0; j < getDirect3DDrivers()->count(); j++ )
			{
				D3D9Driver* curDriver = getDirect3DDrivers()->item(j);
				if( curDriver->DriverDescription() == opt->second.currentValue )
				{
					driver = curDriver;
					break;
				}
			}

			if (driver)
			{
				opt = mOptions.find( "Video Mode" );
				optVideoMode = &opt->second;
				optVideoMode->possibleValues.clear();
				// get vide modes for this device
				for( unsigned k=0; k < driver->getVideoModeList()->count(); k++ )
				{
					videoMode = driver->getVideoModeList()->item( k );
					optVideoMode->possibleValues.push_back( videoMode->getDescription() );
				}

				// Reset video mode to default if previous doesn't avail in new possible values
				std::vector<CamelotEngine::String>::const_iterator itValue =
					std::find(optVideoMode->possibleValues.begin(),
					optVideoMode->possibleValues.end(),
					optVideoMode->currentValue);
				if (itValue == optVideoMode->possibleValues.end())
				{
					optVideoMode->currentValue = "800 x 600 @ 32-bit colour";
				}

				// Also refresh FSAA options
				refreshFSAAOptions();
			}
		}

	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::refreshFSAAOptions()
	{
		ConfigOptionMap::iterator it = mOptions.find( "FSAA" );
		ConfigOption* optFSAA = &it->second;
		optFSAA->possibleValues.clear();
		optFSAA->possibleValues.push_back("0");

		it = mOptions.find("Rendering Device");
		D3D9Driver *driver = getDirect3DDrivers()->item(it->second.currentValue);
		if (driver)
		{
			it = mOptions.find("Video Mode");
			D3D9VideoMode *videoMode = driver->getVideoModeList()->item(it->second.currentValue);
			if (videoMode)
			{
				DWORD numLevels = 0;
				bool bOK;

				for (unsigned int n = 2; n < 25; n++)
				{
					bOK = this->_checkMultiSampleQuality(
						(D3DMULTISAMPLE_TYPE)n, 
						&numLevels, 
						videoMode->getFormat(), 
						driver->getAdapterNumber(),
						D3DDEVTYPE_HAL,
						TRUE);
					if (bOK)
					{
						optFSAA->possibleValues.push_back(toString(n));
						if (n >= 8)
							optFSAA->possibleValues.push_back(toString(n) + " [Quality]");
					}
				}

			}
		}

		// Reset FSAA to none if previous doesn't avail in new possible values
		std::vector<CamelotEngine::String>::const_iterator itValue =
			std::find(optFSAA->possibleValues.begin(),
			optFSAA->possibleValues.end(),
			optFSAA->currentValue);
		if (itValue == optFSAA->possibleValues.end())
		{
			optFSAA->currentValue = "0";
		}

	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::shutdown()
	{
		RenderSystem::shutdown();
		
		SAFE_DELETE( mDeviceManager );

		SAFE_DELETE( mDriverList );
		mActiveD3DDriver = NULL;	
						
		TextureManager::shutDown();
		HardwareBufferManager::shutDown();
		GpuProgramManager::shutDown();		
	}
	//---------------------------------------------------------------------
	RenderSystemCapabilities* D3D9RenderSystem::updateRenderSystemCapabilities(D3D9RenderWindow* renderWindow)
	{			
		RenderSystemCapabilities* rsc = mCurrentCapabilities;
		if (rsc == NULL)
			rsc = new RenderSystemCapabilities();

		rsc->setCategoryRelevant(CAPS_CATEGORY_D3D9, true);
		rsc->setDriverVersion(mDriverVersion);
		rsc->setDeviceName(mActiveD3DDriver->DriverDescription());
		rsc->setRenderSystemName(getName());

		// Supports fixed-function
		rsc->setCapability(RSC_FIXED_FUNCTION);
			
				
		// Init caps to maximum.		
		rsc->setNumTextureUnits(1024);
		rsc->setCapability(RSC_ANISOTROPY);
		rsc->setCapability(RSC_AUTOMIPMAP);
		rsc->setCapability(RSC_DOT3);
		rsc->setCapability(RSC_CUBEMAPPING);		
		rsc->setCapability(RSC_SCISSOR_TEST);		
		rsc->setCapability(RSC_TWO_SIDED_STENCIL);		
		rsc->setCapability(RSC_STENCIL_WRAP);
		rsc->setCapability(RSC_HWOCCLUSION);		
		rsc->setCapability(RSC_USER_CLIP_PLANES);			
		rsc->setCapability(RSC_VERTEX_FORMAT_UBYTE4);			
		rsc->setCapability(RSC_TEXTURE_3D);			
		rsc->setCapability(RSC_NON_POWER_OF_2_TEXTURES);
		rsc->setNonPOW2TexturesLimited(false);
		rsc->setNumMultiRenderTargets(CM_MAX_MULTIPLE_RENDER_TARGETS);
		rsc->setCapability(RSC_MRT_DIFFERENT_BIT_DEPTHS);		
		rsc->setCapability(RSC_POINT_SPRITES);			
		rsc->setCapability(RSC_POINT_EXTENDED_PARAMETERS);								
		rsc->setMaxPointSize(10.0);
		rsc->setCapability(RSC_MIPMAP_LOD_BIAS);				
		rsc->setCapability(RSC_PERSTAGECONSTANT);
		rsc->setCapability(RSC_HWSTENCIL);
		rsc->setStencilBufferBitDepth(8);
		rsc->setCapability(RSC_ADVANCED_BLEND_OPERATIONS);

		for (UINT32 i=0; i < mDeviceManager->getDeviceCount(); ++i)
		{
			D3D9Device* device			 = mDeviceManager->getDevice(i);
			IDirect3DDevice9* d3d9Device = device->getD3D9Device();

			IDirect3DSurface9* pSurf;
			

			// Check for hardware stencil support
			d3d9Device->GetDepthStencilSurface(&pSurf);

			if (pSurf != NULL)
			{
				D3DSURFACE_DESC surfDesc;

				pSurf->GetDesc(&surfDesc);
				pSurf->Release();

				if (surfDesc.Format != D3DFMT_D15S1 &&
					surfDesc.Format != D3DFMT_D24S8 && 				
					surfDesc.Format != D3DFMT_D24X4S4 && 
					surfDesc.Format != D3DFMT_D24FS8)			
					rsc->unsetCapability(RSC_HWSTENCIL);	
			}																	

			// Check for hardware occlusion support
			HRESULT hr = d3d9Device->CreateQuery(D3DQUERYTYPE_OCCLUSION,  NULL);

			if (FAILED(hr))
				rsc->unsetCapability(RSC_HWOCCLUSION);
		}
		
		// Update RS caps using the minimum value found in adapter list.
		for (unsigned int i=0; i < mDriverList->count(); ++i)
		{
			D3D9Driver* pCurDriver       = mDriverList->item(i);			
			const D3DCAPS9& rkCurCaps    = pCurDriver->getD3D9DeviceCaps();

			if (rkCurCaps.MaxSimultaneousTextures < rsc->getNumTextureUnits())
			{
				rsc->setNumTextureUnits(static_cast<UINT16>(rkCurCaps.MaxSimultaneousTextures));
			}

			// Check for Anisotropy.
			if (rkCurCaps.MaxAnisotropy <= 1)
				rsc->unsetCapability(RSC_ANISOTROPY);

			// Check automatic mipmap generation.
			if ((rkCurCaps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP) == 0)
				rsc->unsetCapability(RSC_AUTOMIPMAP);

			// Check Dot product 3.
			if ((rkCurCaps.TextureOpCaps & D3DTEXOPCAPS_DOTPRODUCT3) == 0)
				rsc->unsetCapability(RSC_DOT3);

			// Scissor test
			if ((rkCurCaps.RasterCaps & D3DPRASTERCAPS_SCISSORTEST) == 0)
				rsc->unsetCapability(RSC_SCISSOR_TEST);


			// Two-sided stencil
			if ((rkCurCaps.StencilCaps & D3DSTENCILCAPS_TWOSIDED) == 0)
				rsc->unsetCapability(RSC_TWO_SIDED_STENCIL);

			// stencil wrap
			if ((rkCurCaps.StencilCaps & D3DSTENCILCAPS_INCR) == 0 ||
				(rkCurCaps.StencilCaps & D3DSTENCILCAPS_DECR) == 0)
				rsc->unsetCapability(RSC_STENCIL_WRAP);

			// User clip planes
			if (rkCurCaps.MaxUserClipPlanes == 0)			
				rsc->unsetCapability(RSC_USER_CLIP_PLANES);			

			// UBYTE4 type?
			if ((rkCurCaps.DeclTypes & D3DDTCAPS_UBYTE4) == 0)			
				rsc->unsetCapability(RSC_VERTEX_FORMAT_UBYTE4);	

			// Check cube map support.
			if ((rkCurCaps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP) == 0)
				rsc->unsetCapability(RSC_CUBEMAPPING);
			
			// 3D textures?
			if ((rkCurCaps.TextureCaps & D3DPTEXTURECAPS_VOLUMEMAP) == 0)			
				rsc->unsetCapability(RSC_TEXTURE_3D);			

			if (rkCurCaps.TextureCaps & D3DPTEXTURECAPS_POW2)
			{
				// Conditional support for non POW2
				if (rkCurCaps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL)				
					rsc->setNonPOW2TexturesLimited(true);				

				// Only power of 2 supported.
				else					
					rsc->unsetCapability(RSC_NON_POWER_OF_2_TEXTURES);				
			}	

			// Number of render targets
			if (rkCurCaps.NumSimultaneousRTs < rsc->getNumMultiRenderTargets())
			{
				rsc->setNumMultiRenderTargets(std::min((UINT16)rkCurCaps.NumSimultaneousRTs, (UINT16)CM_MAX_MULTIPLE_RENDER_TARGETS));
			}	

			if((rkCurCaps.PrimitiveMiscCaps & D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS) == 0)
			{
				rsc->unsetCapability(RSC_MRT_DIFFERENT_BIT_DEPTHS);
			}

			// Point sprites 
			if (rkCurCaps.MaxPointSize <= 1.0f)
			{
				rsc->unsetCapability(RSC_POINT_SPRITES);
				// sprites and extended parameters go together in D3D
				rsc->unsetCapability(RSC_POINT_EXTENDED_PARAMETERS);				
			}
			
			// Take the minimum point size.
			if (rkCurCaps.MaxPointSize < rsc->getMaxPointSize())
				rsc->setMaxPointSize(rkCurCaps.MaxPointSize);	

			// Mipmap LOD biasing?
			if ((rkCurCaps.RasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS) == 0)			
				rsc->unsetCapability(RSC_MIPMAP_LOD_BIAS);			


			// Do we support per-stage src_manual constants?
			// HACK - ATI drivers seem to be buggy and don't support per-stage constants properly?
			// TODO: move this to RSC
			if((rkCurCaps.PrimitiveMiscCaps & D3DPMISCCAPS_PERSTAGECONSTANT) == 0)
				rsc->unsetCapability(RSC_PERSTAGECONSTANT);

			// Advanced blend operations? min max subtract rev 
			if((rkCurCaps.PrimitiveMiscCaps & D3DPMISCCAPS_BLENDOP) == 0)
				rsc->unsetCapability(RSC_ADVANCED_BLEND_OPERATIONS);
		}				
									
		// Blending between stages supported
		rsc->setCapability(RSC_BLENDING);
		

		// We always support compression, D3DX will decompress if device does not support
		rsc->setCapability(RSC_TEXTURE_COMPRESSION);
		rsc->setCapability(RSC_TEXTURE_COMPRESSION_DXT);

		// We always support VBOs
		rsc->setCapability(RSC_VBO);

			
		convertVertexShaderCaps(rsc);
		convertPixelShaderCaps(rsc);

		// Adapter details
		const D3DADAPTER_IDENTIFIER9& adapterID = mActiveD3DDriver->getAdapterIdentifier();

		// determine vendor
		// Full list of vendors here: http://www.pcidatabase.com/vendors.php?sort=id
		switch(adapterID.VendorId)
		{
		case 0x10DE:
			rsc->setVendor(GPU_NVIDIA);
			break;
		case 0x1002:
			rsc->setVendor(GPU_ATI);
			break;
		case 0x163C:
		case 0x8086:
			rsc->setVendor(GPU_INTEL);
			break;
		case 0x5333:
			rsc->setVendor(GPU_S3);
			break;
		case 0x3D3D:
			rsc->setVendor(GPU_3DLABS);
			break;
		case 0x102B:
			rsc->setVendor(GPU_MATROX);
			break;
		case 0x1039:
			rsc->setVendor(GPU_SIS);
			break;
		default:
			rsc->setVendor(GPU_UNKNOWN);
			break;
		};

		// Infinite projection?
		// We have no capability for this, so we have to base this on our
		// experience and reports from users
		// Non-vertex program capable hardware does not appear to support it
		if (rsc->hasCapability(RSC_VERTEX_PROGRAM))
		{
			// GeForce4 Ti (and presumably GeForce3) does not
			// render infinite projection properly, even though it does in GL
			// So exclude all cards prior to the FX range from doing infinite
			if (rsc->getVendor() != GPU_NVIDIA || // not nVidia
				!((adapterID.DeviceId >= 0x200 && adapterID.DeviceId <= 0x20F) || //gf3
				(adapterID.DeviceId >= 0x250 && adapterID.DeviceId <= 0x25F) || //gf4ti
				(adapterID.DeviceId >= 0x280 && adapterID.DeviceId <= 0x28F) || //gf4ti
				(adapterID.DeviceId >= 0x170 && adapterID.DeviceId <= 0x18F) || //gf4 go
				(adapterID.DeviceId >= 0x280 && adapterID.DeviceId <= 0x28F)))  //gf4ti go
			{
				rsc->setCapability(RSC_INFINITE_FAR_PLANE);
			}

		}
	
		// We always support rendertextures bigger than the frame buffer
		rsc->setCapability(RSC_HWRENDER_TO_TEXTURE);

		// Determine if any floating point texture format is supported
		D3DFORMAT floatFormats[6] = {D3DFMT_R16F, D3DFMT_G16R16F, 
			D3DFMT_A16B16G16R16F, D3DFMT_R32F, D3DFMT_G32R32F, 
			D3DFMT_A32B32G32R32F};
		IDirect3DSurface9* bbSurf;
		renderWindow->getCustomAttribute("DDBACKBUFFER", &bbSurf);
		D3DSURFACE_DESC bbSurfDesc;
		bbSurf->GetDesc(&bbSurfDesc);

		for (int i = 0; i < 6; ++i)
		{
			if (SUCCEEDED(mpD3D->CheckDeviceFormat(mActiveD3DDriver->getAdapterNumber(), 
				D3DDEVTYPE_HAL, bbSurfDesc.Format, 
				0, D3DRTYPE_TEXTURE, floatFormats[i])))
			{
				rsc->setCapability(RSC_TEXTURE_FLOAT);
				break;
			}

		}

	
		// TODO: make convertVertex/Fragment fill in rsc
		// TODO: update the below line to use rsc
		// Vertex textures
		if (rsc->isShaderProfileSupported("vs_3_0"))
		{
			// Run through all the texture formats looking for any which support
			// vertex texture fetching. Must have at least one!
			// All ATI Radeon up to X1n00 say they support vs_3_0, 
			// but they support no texture formats for vertex texture fetch (cheaters!)
			if (checkVertexTextureFormats(renderWindow))
			{
				rsc->setCapability(RSC_VERTEX_TEXTURE_FETCH);
				// always 4 vertex texture units in vs_3_0, and never shared
				rsc->setNumVertexTextureUnits(4);
				rsc->setVertexTextureUnitsShared(false);
			}
		}		

		// Check alpha to coverage support
		// this varies per vendor! But at least SM3 is required
		if (rsc->isShaderProfileSupported("ps_3_0"))
		{
			// NVIDIA needs a separate check
			if (rsc->getVendor() == GPU_NVIDIA)
			{
				if (mpD3D->CheckDeviceFormat(
						D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0,D3DRTYPE_SURFACE, 
						(D3DFORMAT)MAKEFOURCC('A', 'T', 'O', 'C')) == S_OK)
				{
					rsc->setCapability(RSC_ALPHA_TO_COVERAGE);
				}

			}
			else if (rsc->getVendor() == GPU_ATI)
			{
				// There is no check on ATI, we have to assume SM3 == support
				rsc->setCapability(RSC_ALPHA_TO_COVERAGE);
			}

			// no other cards have Dx9 hacks for alpha to coverage, as far as I know
		}


		if (mCurrentCapabilities == NULL)
		{		
			mCurrentCapabilities = rsc;
			mCurrentCapabilities->addShaderProfile("hlsl");
			mCurrentCapabilities->addShaderProfile("cg");

			initialiseFromRenderSystemCapabilities(mCurrentCapabilities, renderWindow);
		}

		return rsc;
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::convertVertexShaderCaps(RenderSystemCapabilities* rsc) const
	{
		UINT16 major = 0xFF;
		UINT16 minor = 0xFF;
		D3DCAPS9 minVSCaps;

		// Find the device with the lowest vertex shader caps.
		for (unsigned int i=0; i < mDriverList->count(); ++i)
		{
			D3D9Driver* pCurDriver      = mDriverList->item(i);			
			const D3DCAPS9& rkCurCaps   = pCurDriver->getD3D9DeviceCaps();
			UINT16 currMajor			= static_cast<UINT16>((rkCurCaps.VertexShaderVersion & 0x0000FF00) >> 8);
			UINT16 currMinor			= static_cast<UINT16>(rkCurCaps.VertexShaderVersion & 0x000000FF);

			if (currMajor < major)	
			{
				major = currMajor;
				minor = currMinor;
				minVSCaps = rkCurCaps;
			}
			else if (currMajor == major && currMinor < minor)
			{
				minor = currMinor;
				minVSCaps = rkCurCaps;
			}			
		}

		// In case we didn't found any vertex shader support
		// try the IDirect3DDevice9 caps instead of the IDirect3D9
		// software vertex processing is reported there
		if (major == 0 && minor == 0)
		{
			IDirect3DDevice9* lpD3DDevice9 = getActiveD3D9Device();
			D3DCAPS9 d3dDeviceCaps9;
			lpD3DDevice9->GetDeviceCaps(&d3dDeviceCaps9);
			major = static_cast<UINT16>((d3dDeviceCaps9.VertexShaderVersion & 0x0000FF00) >> 8);
			minor = static_cast<UINT16>(d3dDeviceCaps9.VertexShaderVersion & 0x000000FF);
		}
		
		bool vs2x = false;
		bool vs2a = false;

		// Special case detection for vs_2_x/a support
		if (major >= 2)
		{
			if ((minVSCaps.VS20Caps.Caps & D3DVS20CAPS_PREDICATION) &&
				(minVSCaps.VS20Caps.DynamicFlowControlDepth > 0) &&
				(minVSCaps.VS20Caps.NumTemps >= 12))
			{
				vs2x = true;
			}

			if ((minVSCaps.VS20Caps.Caps & D3DVS20CAPS_PREDICATION) &&
				(minVSCaps.VS20Caps.DynamicFlowControlDepth > 0) &&
				(minVSCaps.VS20Caps.NumTemps >= 13))
			{
				vs2a = true;
			}
		}

		// Populate max param count
		switch (major)
		{
		case 1:
			// No boolean params allowed
			rsc->setVertexProgramConstantBoolCount(0);
			// No integer params allowed
			rsc->setVertexProgramConstantIntCount(0);
			// float params, always 4D
			rsc->setVertexProgramConstantFloatCount(static_cast<UINT16>(minVSCaps.MaxVertexShaderConst));

			break;
		case 2:
			// 16 boolean params allowed
			rsc->setVertexProgramConstantBoolCount(16);
			// 16 integer params allowed, 4D
			rsc->setVertexProgramConstantIntCount(16);
			// float params, always 4D
			rsc->setVertexProgramConstantFloatCount(static_cast<UINT16>(minVSCaps.MaxVertexShaderConst));
			break;
		case 3:
			// 16 boolean params allowed
			rsc->setVertexProgramConstantBoolCount(16);
			// 16 integer params allowed, 4D
			rsc->setVertexProgramConstantIntCount(16);
			// float params, always 4D
			rsc->setVertexProgramConstantFloatCount(static_cast<UINT16>(minVSCaps.MaxVertexShaderConst));
			break;
		}

		// populate syntax codes in program manager (no breaks in this one so it falls through)
		switch(major)
		{
		case 3:
			rsc->addShaderProfile("vs_3_0");
			rsc->addGpuProgramProfile(GPP_VS_3_0, "vs_3_0");
		case 2:
			if (vs2x)
			{
				rsc->addShaderProfile("vs_2_x");
				rsc->addGpuProgramProfile(GPP_VS_2_x, "vs_2_x");
			}
			if (vs2a)
			{
				rsc->addShaderProfile("vs_2_a");
				rsc->addGpuProgramProfile(GPP_VS_2_a, "vs_2_a");
			}

			rsc->addShaderProfile("vs_2_0");
			rsc->addGpuProgramProfile(GPP_VS_2_0, "vs_2_0");
		case 1:
			rsc->addShaderProfile("vs_1_1");
			rsc->addGpuProgramProfile(GPP_VS_1_1, "vs_1_1");
			rsc->setCapability(RSC_VERTEX_PROGRAM);
		}
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::convertPixelShaderCaps(RenderSystemCapabilities* rsc) const
	{
		UINT16 major = 0xFF;
		UINT16 minor = 0xFF;
		D3DCAPS9 minPSCaps;

		// Find the device with the lowest pixel shader caps.
		for (unsigned int i=0; i < mDriverList->count(); ++i)
		{
			D3D9Driver* pCurDriver      = mDriverList->item(i);			
			const D3DCAPS9& currCaps    = pCurDriver->getD3D9DeviceCaps();
			UINT16 currMajor			= static_cast<UINT16>((currCaps.PixelShaderVersion & 0x0000FF00) >> 8);
			UINT16 currMinor			= static_cast<UINT16>(currCaps.PixelShaderVersion & 0x000000FF);

			if (currMajor < major)	
			{
				major = currMajor;
				minor = currMinor;
				minPSCaps = currCaps;
			}
			else if (currMajor == major && currMinor < minor)
			{
				minor = currMinor;
				minPSCaps = currCaps;
			}			
		}
		
		bool ps2a = false;
		bool ps2b = false;
		bool ps2x = false;

		// Special case detection for ps_2_x/a/b support
		if (major >= 2)
		{
			if ((minPSCaps.PS20Caps.Caps & D3DPS20CAPS_NOTEXINSTRUCTIONLIMIT) &&
				(minPSCaps.PS20Caps.NumTemps >= 32))
			{
				ps2b = true;
			}

			if ((minPSCaps.PS20Caps.Caps & D3DPS20CAPS_NOTEXINSTRUCTIONLIMIT) &&
				(minPSCaps.PS20Caps.Caps & D3DPS20CAPS_NODEPENDENTREADLIMIT) &&
				(minPSCaps.PS20Caps.Caps & D3DPS20CAPS_ARBITRARYSWIZZLE) &&
				(minPSCaps.PS20Caps.Caps & D3DPS20CAPS_GRADIENTINSTRUCTIONS) &&
				(minPSCaps.PS20Caps.Caps & D3DPS20CAPS_PREDICATION) &&
				(minPSCaps.PS20Caps.NumTemps >= 22))
			{
				ps2a = true;
			}

			// Does this enough?
			if (ps2a || ps2b)
			{
				ps2x = true;
			}
		}

		switch (major)
		{
		case 1:
			// no boolean params allowed
			rsc->setFragmentProgramConstantBoolCount(0);
			// no integer params allowed
			rsc->setFragmentProgramConstantIntCount(0);
			// float params, always 4D
			// NB in ps_1_x these are actually stored as fixed point values,
			// but they are entered as floats
			rsc->setFragmentProgramConstantFloatCount(8);
			break;
		case 2:
			// 16 boolean params allowed
			rsc->setFragmentProgramConstantBoolCount(16);
			// 16 integer params allowed, 4D
			rsc->setFragmentProgramConstantIntCount(16);
			// float params, always 4D
			rsc->setFragmentProgramConstantFloatCount(32);
			break;
		case 3:
			// 16 boolean params allowed
			rsc->setFragmentProgramConstantBoolCount(16);
			// 16 integer params allowed, 4D
			rsc->setFragmentProgramConstantIntCount(16);
			// float params, always 4D
			rsc->setFragmentProgramConstantFloatCount(224);
			break;
		}

		// populate syntax codes in program manager (no breaks in this one so it falls through)
		switch(major)
		{
		case 3:
			if (minor > 0)
			{
				rsc->addShaderProfile("ps_3_x");
				rsc->addGpuProgramProfile(GPP_PS_3_x, "ps_3_x");
			}

			rsc->addShaderProfile("ps_3_0");
			rsc->addGpuProgramProfile(GPP_PS_3_0, "ps_3_0");
		case 2:
			if (ps2x)
			{
				rsc->addShaderProfile("ps_2_x");
				rsc->addGpuProgramProfile(GPP_PS_2_x, "ps_2_x");
			}
			if (ps2a)
			{
				rsc->addShaderProfile("ps_2_a");
				rsc->addGpuProgramProfile(GPP_PS_2_a, "ps_2_a");
			}
			if (ps2b)
			{
				rsc->addShaderProfile("ps_2_b");
				rsc->addGpuProgramProfile(GPP_PS_2_b, "ps_2_b");
			}

			rsc->addShaderProfile("ps_2_0");
			rsc->addGpuProgramProfile(GPP_PS_2_0, "ps_2_0");
		case 1:
			if (major > 1 || minor >= 4)
			{
				rsc->addShaderProfile("ps_1_4");
				rsc->addGpuProgramProfile(GPP_PS_1_4, "ps_1_4");
			}
			if (major > 1 || minor >= 3)
			{
				rsc->addShaderProfile("ps_1_3");
				rsc->addGpuProgramProfile(GPP_PS_1_3, "ps_1_3");
			}
			if (major > 1 || minor >= 2)
			{
				rsc->addShaderProfile("ps_1_2");
				rsc->addGpuProgramProfile(GPP_PS_1_2, "ps_1_2");
			}

			rsc->addShaderProfile("ps_1_1");
			rsc->addGpuProgramProfile(GPP_PS_1_1, "ps_1_1");
			rsc->setCapability(RSC_FRAGMENT_PROGRAM);
		}
	}
	//-----------------------------------------------------------------------
	bool D3D9RenderSystem::checkVertexTextureFormats(D3D9RenderWindow* renderWindow) const
	{
		bool anySupported = false;

		IDirect3DSurface9* bbSurf;
		renderWindow->getCustomAttribute("DDBACKBUFFER", &bbSurf);
		D3DSURFACE_DESC bbSurfDesc;
		bbSurf->GetDesc(&bbSurfDesc);

		for (UINT32 ipf = static_cast<UINT32>(PF_L8); ipf < static_cast<UINT32>(PF_COUNT); ++ipf)
		{
			PixelFormat pf = (PixelFormat)ipf;

			D3DFORMAT fmt = 
				D3D9Mappings::_getPF(D3D9Mappings::_getClosestSupportedPF(pf));

			if (SUCCEEDED(mpD3D->CheckDeviceFormat(
				mActiveD3DDriver->getAdapterNumber(), D3DDEVTYPE_HAL, bbSurfDesc.Format, 
				D3DUSAGE_QUERY_VERTEXTEXTURE, D3DRTYPE_TEXTURE, fmt)))
			{
				// cool, at least one supported
				anySupported = true;
			}
		}

		return anySupported;

	}
	//-----------------------------------------------------------------------
	void D3D9RenderSystem::initialiseFromRenderSystemCapabilities(RenderSystemCapabilities* caps, RenderTarget* primary)
	{
		if (caps->getRenderSystemName() != getName())
		{
			CM_EXCEPT(InvalidParametersException, 
				"Trying to initialize D3D9RenderSystem from RenderSystemCapabilities that do not support Direct3D9");
		}

		if (caps->isShaderProfileSupported("hlsl"))
			HighLevelGpuProgramManager::instance().addFactory(mHLSLProgramFactory);

		if (caps->isShaderProfileSupported("cg"))
			HighLevelGpuProgramManager::instance().addFactory(mCgProgramFactory);
	}

	//-----------------------------------------------------------------------
	bool D3D9RenderSystem::_checkTextureFilteringSupported(TextureType ttype, PixelFormat format, int usage)
	{
		// Gets D3D format
		D3DFORMAT d3dPF = D3D9Mappings::_getPF(format);
		if (d3dPF == D3DFMT_UNKNOWN)
			return false;

		for (UINT32 i = 0; i < mDeviceManager->getDeviceCount(); ++i)
		{
			D3D9Device* currDevice = mDeviceManager->getDevice(i);
			D3D9RenderWindow* currDevicePrimaryWindow = currDevice->getPrimaryWindow();
			IDirect3DSurface9* pSurface = currDevicePrimaryWindow->getRenderSurface();
			D3DSURFACE_DESC srfDesc;
			
			// Get surface desc
			if (FAILED(pSurface->GetDesc(&srfDesc)))
				return false;

			// Calculate usage
			DWORD d3dusage = D3DUSAGE_QUERY_FILTER;
			if (usage & TU_RENDERTARGET) 
				d3dusage |= D3DUSAGE_RENDERTARGET;
			if (usage & TU_DYNAMIC)
				d3dusage |= D3DUSAGE_DYNAMIC;

			// Detect resource type
			D3DRESOURCETYPE rtype;
			switch(ttype)
			{
			case TEX_TYPE_1D:
			case TEX_TYPE_2D:
				rtype = D3DRTYPE_TEXTURE;
				break;
			case TEX_TYPE_3D:
				rtype = D3DRTYPE_VOLUMETEXTURE;
				break;
			case TEX_TYPE_CUBE_MAP:
				rtype = D3DRTYPE_CUBETEXTURE;
				break;
			default:
				return false;
			}

			HRESULT hr = mpD3D->CheckDeviceFormat(
				currDevice->getAdapterNumber(),
				currDevice->getDeviceType(),
				srfDesc.Format,
				d3dusage,
				rtype,
				d3dPF);

			if (FAILED(hr))
				return false;
		}
		
		return true;		
	}
	//-----------------------------------------------------------------------
	MultiRenderTarget * D3D9RenderSystem::createMultiRenderTarget_internal(const String & name)
	{
		MultiRenderTarget *retval;
		retval = new D3D9MultiRenderTarget(name);
		attachRenderTarget_internal(*retval);

		return retval;
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::destroyRenderTarget_internal(const String& name)
	{		
		D3D9RenderWindow* renderWindow = NULL;

	
		// Check render windows
		D3D9RenderWindowList::iterator sw;
		for (sw = mRenderWindows.begin(); sw != mRenderWindows.end(); ++sw)
		{
			if ((*sw)->getName() == name)
			{
				renderWindow = (*sw);					
				mRenderWindows.erase(sw);
				break;
			}
		}
		

		// Do the real removal
		RenderSystem::destroyRenderTarget_internal(name);	
	}

	//---------------------------------------------------------------------
	String D3D9RenderSystem::getErrorDescription_internal( long errorNumber ) const
	{
		const String errMsg = DXGetErrorDescription( errorNumber );
		return errMsg;
	}
	//---------------------------------------------------------------------
	VertexElementType D3D9RenderSystem::getColorVertexElementType() const
	{
		return VET_COLOUR_ARGB;
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::convertProjectionMatrix(const Matrix4& matrix,
		Matrix4& dest, bool forGpuProgram)
	{
		dest = matrix;

		// Convert depth range from [-1,+1] to [0,1]
		dest[2][0] = (dest[2][0] + dest[3][0]) / 2;
		dest[2][1] = (dest[2][1] + dest[3][1]) / 2;
		dest[2][2] = (dest[2][2] + dest[3][2]) / 2;
		dest[2][3] = (dest[2][3] + dest[3][3]) / 2;

		if (!forGpuProgram)
		{
			// Convert right-handed to left-handed
			dest[0][2] = -dest[0][2];
			dest[1][2] = -dest[1][2];
			dest[2][2] = -dest[2][2];
			dest[3][2] = -dest[3][2];
		}
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setPointParameters_internal(float size, 
		bool attenuationEnabled, float constant, float linear, float quadratic,
		float minSize, float maxSize)
	{
		if(attenuationEnabled)
		{
			// scaling required
			__SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
			__SetFloatRenderState(D3DRS_POINTSCALE_A, constant);
			__SetFloatRenderState(D3DRS_POINTSCALE_B, linear);
			__SetFloatRenderState(D3DRS_POINTSCALE_C, quadratic);
		}
		else
		{
			// no scaling required
			__SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
		}
		__SetFloatRenderState(D3DRS_POINTSIZE, size);
		__SetFloatRenderState(D3DRS_POINTSIZE_MIN, minSize);
		if (maxSize == 0.0f)
			maxSize = mCurrentCapabilities->getMaxPointSize();
		__SetFloatRenderState(D3DRS_POINTSIZE_MAX, maxSize);


	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setTexture_internal( size_t stage, bool enabled, const TexturePtr& tex )
	{
		HRESULT hr;
		D3D9TexturePtr dt = std::static_pointer_cast<D3D9Texture>(tex);
		if (enabled && (dt != nullptr))
		{
			IDirect3DBaseTexture9 *pTex = dt->getTexture();
			if (mTexStageDesc[stage].pTex != pTex)
			{
				hr = getActiveD3D9Device()->SetTexture(static_cast<DWORD>(stage), pTex);
				if( hr != S_OK )
				{
					String str = "Unable to set texture in D3D9";
					CM_EXCEPT(RenderingAPIException, str);
				}

				// set stage desc.
				mTexStageDesc[stage].pTex = pTex;
				mTexStageDesc[stage].texType = D3D9Mappings::get(dt->getTextureType());

				// Set gamma now too
				if (dt->isHardwareGammaReadToBeUsed())
				{
					__SetSamplerState(static_cast<DWORD>(stage), D3DSAMP_SRGBTEXTURE, TRUE);
				}
				else
				{
					__SetSamplerState(static_cast<DWORD>(stage), D3DSAMP_SRGBTEXTURE, FALSE);
				}
			}
		}
		else
		{
			if (mTexStageDesc[stage].pTex != 0)
			{
				hr = getActiveD3D9Device()->SetTexture(static_cast<DWORD>(stage), 0);
				if( hr != S_OK )
				{
					String str = "Unable to disable texture '" + toString(stage) + "' in D3D9";
					CM_EXCEPT(RenderingAPIException, str);
				}
			}

			hr = __SetTextureStageState(static_cast<DWORD>(stage), D3DTSS_COLOROP, D3DTOP_DISABLE);
			if( hr != S_OK )
			{
				String str = "Unable to disable texture '" + toString(stage) + "' in D3D9";
				CM_EXCEPT(RenderingAPIException, str);
			}

			// set stage desc. to defaults
			mTexStageDesc[stage].pTex = 0;
			mTexStageDesc[stage].autoTexCoordType = TEXCALC_NONE;
			mTexStageDesc[stage].coordIndex = 0;
			mTexStageDesc[stage].texType = D3D9Mappings::D3D_TEX_TYPE_NORMAL;
		}
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setVertexTexture_internal(size_t stage, const TexturePtr& tex)
	{
		if (tex == nullptr)
		{

			if (mTexStageDesc[stage].pVertexTex != 0)
			{
				HRESULT hr = getActiveD3D9Device()->SetTexture(D3DVERTEXTEXTURESAMPLER0 + static_cast<DWORD>(stage), 0);
				if( hr != S_OK )
				{
					String str = "Unable to disable vertex texture '" 
						+ toString(stage) + "' in D3D9";
					CM_EXCEPT(RenderingAPIException, str);
				}
			}

			// set stage desc. to defaults
			mTexStageDesc[stage].pVertexTex = 0;
		}
		else
		{
			D3D9TexturePtr dt = std::static_pointer_cast<D3D9Texture>(tex);

			IDirect3DBaseTexture9 *pTex = dt->getTexture();
			if (mTexStageDesc[stage].pVertexTex != pTex)
			{
				HRESULT hr = getActiveD3D9Device()->SetTexture(D3DVERTEXTEXTURESAMPLER0 + static_cast<DWORD>(stage), pTex);
				if( hr != S_OK )
				{
					String str = "Unable to set vertex texture in D3D9";
					CM_EXCEPT(RenderingAPIException, str);
				}

				// set stage desc.
				mTexStageDesc[stage].pVertexTex = pTex;
			}

		}

	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::disableTextureUnit_internal(size_t texUnit)
	{
		RenderSystem::disableTextureUnit_internal(texUnit);
		// also disable vertex texture unit
		static TexturePtr nullPtr;
		setVertexTexture_internal(texUnit, nullPtr);
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setTextureMipmapBias_internal(size_t unit, float bias)
	{
		if (mCurrentCapabilities->hasCapability(RSC_MIPMAP_LOD_BIAS))
		{
			// ugh - have to pass float data through DWORD with no conversion
			HRESULT hr = __SetSamplerState(static_cast<DWORD>(unit), D3DSAMP_MIPMAPLODBIAS, 
				*(DWORD*)&bias);
			if(FAILED(hr))
				CM_EXCEPT(RenderingAPIException, "Unable to set texture mipmap bias");

		}
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setTextureAddressingMode_internal( size_t stage, 
		const SamplerState::UVWAddressingMode& uvw )
	{
		HRESULT hr;
		if( FAILED( hr = __SetSamplerState( static_cast<DWORD>(stage), D3DSAMP_ADDRESSU, D3D9Mappings::get(uvw.u, mDeviceManager->getActiveDevice()->getD3D9DeviceCaps()) ) ) )
			CM_EXCEPT(RenderingAPIException, "Failed to set texture addressing mode for U" );
		if( FAILED( hr = __SetSamplerState( static_cast<DWORD>(stage), D3DSAMP_ADDRESSV, D3D9Mappings::get(uvw.v, mDeviceManager->getActiveDevice()->getD3D9DeviceCaps()) ) ) )
			CM_EXCEPT(RenderingAPIException, "Failed to set texture addressing mode for V");
		if( FAILED( hr = __SetSamplerState( static_cast<DWORD>(stage), D3DSAMP_ADDRESSW, D3D9Mappings::get(uvw.w, mDeviceManager->getActiveDevice()->getD3D9DeviceCaps()) ) ) )
			CM_EXCEPT(RenderingAPIException, "Failed to set texture addressing mode for W");
	}
	//-----------------------------------------------------------------------------
	void D3D9RenderSystem::setTextureBorderColor_internal(size_t stage,
		const Color& colour)
	{
		HRESULT hr;
		if( FAILED( hr = __SetSamplerState( static_cast<DWORD>(stage), D3DSAMP_BORDERCOLOR, colour.getAsARGB()) ) )
			CM_EXCEPT(RenderingAPIException, "Failed to set texture border colour");
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setSceneBlending_internal( SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendOperation op )
	{
		HRESULT hr;
		if( sourceFactor == SBF_ONE && destFactor == SBF_ZERO)
		{
			if (FAILED(hr = __SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE)))
				CM_EXCEPT(RenderingAPIException, "Failed to set alpha blending option");
		}
		else
		{
			if (FAILED(hr = __SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE)))
				CM_EXCEPT(RenderingAPIException, "Failed to set alpha blending option");
			if (FAILED(hr = __SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE)))
				CM_EXCEPT(RenderingAPIException, "Failed to set separate alpha blending option");
			if( FAILED( hr = __SetRenderState( D3DRS_SRCBLEND, D3D9Mappings::get(sourceFactor) ) ) )
				CM_EXCEPT(RenderingAPIException, "Failed to set source blend");
			if( FAILED( hr = __SetRenderState( D3DRS_DESTBLEND, D3D9Mappings::get(destFactor) ) ) )
				CM_EXCEPT(RenderingAPIException, "Failed to set destination blend");
		}

		if (FAILED(hr = __SetRenderState(D3DRS_BLENDOP, D3D9Mappings::get(op))))
			CM_EXCEPT(RenderingAPIException, "Failed to set scene blending operation option");
		if (FAILED(hr = __SetRenderState(D3DRS_BLENDOPALPHA, D3D9Mappings::get(op))))
			CM_EXCEPT(RenderingAPIException, "Failed to set scene blending operation option");
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setSeparateSceneBlending_internal( SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendFactor sourceFactorAlpha, 
		SceneBlendFactor destFactorAlpha, SceneBlendOperation op, SceneBlendOperation alphaOp )
	{
		HRESULT hr;
		if( sourceFactor == SBF_ONE && destFactor == SBF_ZERO && 
			sourceFactorAlpha == SBF_ONE && destFactorAlpha == SBF_ZERO)
		{
			if (FAILED(hr = __SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE)))
				CM_EXCEPT(RenderingAPIException, "Failed to set alpha blending option");
		}
		else
		{
			if (FAILED(hr = __SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE)))
				CM_EXCEPT(RenderingAPIException, "Failed to set alpha blending option");
			if (FAILED(hr = __SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE)))
				CM_EXCEPT(RenderingAPIException, "Failed to set separate alpha blending option");
			if( FAILED( hr = __SetRenderState( D3DRS_SRCBLEND, D3D9Mappings::get(sourceFactor) ) ) )
				CM_EXCEPT(RenderingAPIException, "Failed to set source blend");
			if( FAILED( hr = __SetRenderState( D3DRS_DESTBLEND, D3D9Mappings::get(destFactor) ) ) )
				CM_EXCEPT(RenderingAPIException, "Failed to set destination blend");
			if( FAILED( hr = __SetRenderState( D3DRS_SRCBLENDALPHA, D3D9Mappings::get(sourceFactorAlpha) ) ) )
				CM_EXCEPT(RenderingAPIException, "Failed to set alpha source blend");
			if( FAILED( hr = __SetRenderState( D3DRS_DESTBLENDALPHA, D3D9Mappings::get(destFactorAlpha) ) ) )
				CM_EXCEPT(RenderingAPIException, "Failed to set alpha destination blend");
		}

		if (FAILED(hr = __SetRenderState(D3DRS_BLENDOP, D3D9Mappings::get(op))))
			CM_EXCEPT(RenderingAPIException, "Failed to set scene blending operation option");
		if (FAILED(hr = __SetRenderState(D3DRS_BLENDOPALPHA, D3D9Mappings::get(alphaOp))))
			CM_EXCEPT(RenderingAPIException, "Failed to set alpha scene blending operation option");
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setAlphaRejectSettings_internal( CompareFunction func, unsigned char value, bool alphaToCoverage )
	{
		HRESULT hr;
		bool a2c = false;
		static bool lasta2c = false;

		if (func != CMPF_ALWAYS_PASS)
		{
			if( FAILED( hr = __SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE ) ) )
				CM_EXCEPT(RenderingAPIException, "Failed to enable alpha testing");

			a2c = alphaToCoverage;
		}
		else
		{
			if( FAILED( hr = __SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE ) ) )
				CM_EXCEPT(RenderingAPIException, "Failed to disable alpha testing");
		}
		// Set always just be sure
		if( FAILED( hr = __SetRenderState( D3DRS_ALPHAFUNC, D3D9Mappings::get(func) ) ) )
			CM_EXCEPT(RenderingAPIException, "Failed to set alpha reject function");
		if( FAILED( hr = __SetRenderState( D3DRS_ALPHAREF, value ) ) )
			CM_EXCEPT(RenderingAPIException, "Failed to set render state D3DRS_ALPHAREF");

		// Alpha to coverage
		if (getCapabilities_internal()->hasCapability(RSC_ALPHA_TO_COVERAGE))
		{
			// Vendor-specific hacks on renderstate, gotta love 'em
			if (getCapabilities_internal()->getVendor() == GPU_NVIDIA)
			{
				if (a2c)
				{
					if( FAILED( hr = __SetRenderState( D3DRS_ADAPTIVETESS_Y,  (D3DFORMAT)MAKEFOURCC('A', 'T', 'O', 'C') ) ) )
						CM_EXCEPT(RenderingAPIException, "Failed to set alpha to coverage option");
				}
				else
				{
					if( FAILED( hr = __SetRenderState( D3DRS_ADAPTIVETESS_Y,  D3DFMT_UNKNOWN ) ) )
						CM_EXCEPT(RenderingAPIException, "Failed to set alpha to coverage option");
				}

			}
			else if ((getCapabilities_internal()->getVendor() == GPU_ATI))
			{
				if (a2c)
				{
					if( FAILED( hr = __SetRenderState( D3DRS_POINTSIZE,  MAKEFOURCC('A','2','M','1') ) ) )
						CM_EXCEPT(RenderingAPIException, "Failed to set alpha to coverage option");
				}
				else
				{
					// discovered this through trial and error, seems to work
					if( FAILED( hr = __SetRenderState( D3DRS_POINTSIZE,  MAKEFOURCC('A','2','M','0') ) ) )
						CM_EXCEPT(RenderingAPIException, "Failed to set alpha to coverage option");
				}
			}
			// no hacks available for any other vendors?
			lasta2c = a2c;
		}

	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setCullingMode_internal( CullingMode mode )
	{
		mCullingMode = mode;
		HRESULT hr;
		bool flip = ((mActiveRenderTarget->requiresTextureFlipping() && !mInvertVertexWinding) ||
			(!mActiveRenderTarget->requiresTextureFlipping() && mInvertVertexWinding));

		if( FAILED (hr = __SetRenderState(D3DRS_CULLMODE, 
			D3D9Mappings::get(mode, flip))) )
			CM_EXCEPT(RenderingAPIException, "Failed to set culling mode");
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setDepthBufferParams_internal( bool depthTest, bool depthWrite, CompareFunction depthFunction )
	{
		setDepthBufferCheckEnabled_internal( depthTest );
		setDepthBufferWriteEnabled_internal( depthWrite );
		setDepthBufferFunction_internal( depthFunction );
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setDepthBufferCheckEnabled_internal( bool enabled )
	{
		HRESULT hr;

		if( enabled )
			hr = __SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
		else
			hr = __SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );

		if( FAILED( hr ) )
			CM_EXCEPT(RenderingAPIException, "Error setting depth buffer test state");
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setDepthBufferWriteEnabled_internal( bool enabled )
	{
		HRESULT hr;

		if( FAILED( hr = __SetRenderState( D3DRS_ZWRITEENABLE, enabled ) ) )
			CM_EXCEPT(RenderingAPIException, "Error setting depth buffer write state");
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setDepthBufferFunction_internal( CompareFunction func )
	{
		HRESULT hr;
		if( FAILED( hr = __SetRenderState( D3DRS_ZFUNC, D3D9Mappings::get(func) ) ) )
			CM_EXCEPT(RenderingAPIException, "Error setting depth buffer test function");
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setDepthBias_internal(float constantBias, float slopeScaleBias)
	{

		if ((mDeviceManager->getActiveDevice()->getD3D9DeviceCaps().RasterCaps & D3DPRASTERCAPS_DEPTHBIAS) != 0)
		{
			// Negate bias since D3D is backward
			// D3D also expresses the constant bias as an absolute value, rather than 
			// relative to minimum depth unit, so scale to fit
			constantBias = -constantBias / 250000.0f;
			HRESULT hr = __SetRenderState(D3DRS_DEPTHBIAS, FLOAT2DWORD(constantBias));
			if (FAILED(hr))
				CM_EXCEPT(RenderingAPIException, "Error setting constant depth bias");
		}

		if ((mDeviceManager->getActiveDevice()->getD3D9DeviceCaps().RasterCaps & D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS) != 0)
		{
			// Negate bias since D3D is backward
			slopeScaleBias = -slopeScaleBias;
			HRESULT hr = __SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, FLOAT2DWORD(slopeScaleBias));
			if (FAILED(hr))
				CM_EXCEPT(RenderingAPIException, "Error setting slope scale depth bias");
		}


	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setColorBufferWriteEnabled_internal(bool red, bool green, 
		bool blue, bool alpha)
	{
		DWORD val = 0;
		if (red) 
			val |= D3DCOLORWRITEENABLE_RED;
		if (green)
			val |= D3DCOLORWRITEENABLE_GREEN;
		if (blue)
			val |= D3DCOLORWRITEENABLE_BLUE;
		if (alpha)
			val |= D3DCOLORWRITEENABLE_ALPHA;
		HRESULT hr = __SetRenderState(D3DRS_COLORWRITEENABLE, val); 
		if (FAILED(hr))
			CM_EXCEPT(RenderingAPIException, "Error setting colour write enable flags");
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setPolygonMode_internal(PolygonMode level)
	{
		HRESULT hr = __SetRenderState(D3DRS_FILLMODE, D3D9Mappings::get(level));
		if (FAILED(hr))
			CM_EXCEPT(RenderingAPIException, "Error setting polygon mode.");
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setStencilCheckEnabled_internal(bool enabled)
	{
		// Allow stencilling
		HRESULT hr = __SetRenderState(D3DRS_STENCILENABLE, enabled);
		if (FAILED(hr))
			CM_EXCEPT(RenderingAPIException, "Error enabling / disabling stencilling.");
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setStencilBufferParams_internal(CompareFunction func, 
		UINT32 refValue, UINT32 mask, StencilOperation stencilFailOp, 
		StencilOperation depthFailOp, StencilOperation passOp, 
		bool twoSidedOperation)
	{
		HRESULT hr;
		bool flip;

		// 2-sided operation
		if (twoSidedOperation)
		{
			if (!mCurrentCapabilities->hasCapability(RSC_TWO_SIDED_STENCIL))
				CM_EXCEPT(InvalidParametersException, "2-sided stencils are not supported");
			hr = __SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, TRUE);
			if (FAILED(hr))
				CM_EXCEPT(RenderingAPIException, "Error setting 2-sided stencil mode.");
			// NB: We should always treat CCW as front face for consistent with default
			// culling mode. Therefore, we must take care with two-sided stencil settings.
			flip = (mInvertVertexWinding && mActiveRenderTarget->requiresTextureFlipping()) ||
				(!mInvertVertexWinding && !mActiveRenderTarget->requiresTextureFlipping());

			// Set alternative versions of ops
			// fail op
			hr = __SetRenderState(D3DRS_CCW_STENCILFAIL, D3D9Mappings::get(stencilFailOp, !flip));
			if (FAILED(hr))
				CM_EXCEPT(RenderingAPIException, "Error setting stencil fail operation (2-sided).");

			// depth fail op
			hr = __SetRenderState(D3DRS_CCW_STENCILZFAIL, D3D9Mappings::get(depthFailOp, !flip));
			if (FAILED(hr))
				CM_EXCEPT(RenderingAPIException, "Error setting stencil depth fail operation (2-sided).");

			// pass op
			hr = __SetRenderState(D3DRS_CCW_STENCILPASS, D3D9Mappings::get(passOp, !flip));
			if (FAILED(hr))
				CM_EXCEPT(RenderingAPIException, "Error setting stencil pass operation (2-sided).");
		}
		else
		{
			hr = __SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, FALSE);
			if (FAILED(hr))
				CM_EXCEPT(RenderingAPIException, "Error setting 1-sided stencil mode.");
			flip = false;
		}

		// func
		hr = __SetRenderState(D3DRS_STENCILFUNC, D3D9Mappings::get(func));
		if (FAILED(hr))
			CM_EXCEPT(RenderingAPIException, "Error setting stencil buffer test function.");

		// reference value
		hr = __SetRenderState(D3DRS_STENCILREF, refValue);
		if (FAILED(hr))
			CM_EXCEPT(RenderingAPIException, "Error setting stencil buffer reference value.");

		// mask
		hr = __SetRenderState(D3DRS_STENCILMASK, mask);
		if (FAILED(hr))
			CM_EXCEPT(RenderingAPIException, "Error setting stencil buffer mask.");

		// fail op
		hr = __SetRenderState(D3DRS_STENCILFAIL, D3D9Mappings::get(stencilFailOp, flip));
		if (FAILED(hr))
			CM_EXCEPT(RenderingAPIException, "Error setting stencil fail operation.");

		// depth fail op
		hr = __SetRenderState(D3DRS_STENCILZFAIL, D3D9Mappings::get(depthFailOp, flip));
		if (FAILED(hr))
			CM_EXCEPT(RenderingAPIException, "Error setting stencil depth fail operation.");

		// pass op
		hr = __SetRenderState(D3DRS_STENCILPASS, D3D9Mappings::get(passOp, flip));
		if (FAILED(hr))
			CM_EXCEPT(RenderingAPIException, "Error setting stencil pass operation.");
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setTextureFiltering_internal(size_t unit, FilterType ftype, 
		FilterOptions filter)
	{
		HRESULT hr;
		D3D9Mappings::eD3DTexType texType = mTexStageDesc[unit].texType;
		hr = __SetSamplerState( static_cast<DWORD>(unit), D3D9Mappings::get(ftype), 
			D3D9Mappings::get(ftype, filter, mDeviceManager->getActiveDevice()->getD3D9DeviceCaps(), texType));
		if (FAILED(hr))
			CM_EXCEPT(RenderingAPIException, "Failed to set texture filter ");
	}
	//---------------------------------------------------------------------
	DWORD D3D9RenderSystem::_getCurrentAnisotropy(size_t unit)
	{
		DWORD oldVal;
		getActiveD3D9Device()->GetSamplerState(static_cast<DWORD>(unit), D3DSAMP_MAXANISOTROPY, &oldVal);
		return oldVal;
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setTextureAnisotropy_internal(size_t unit, unsigned int maxAnisotropy)
	{
		if (static_cast<DWORD>(maxAnisotropy) > mDeviceManager->getActiveDevice()->getD3D9DeviceCaps().MaxAnisotropy)
			maxAnisotropy = mDeviceManager->getActiveDevice()->getD3D9DeviceCaps().MaxAnisotropy;

		if (_getCurrentAnisotropy(unit) != maxAnisotropy)
			__SetSamplerState( static_cast<DWORD>(unit), D3DSAMP_MAXANISOTROPY, maxAnisotropy );
	}
	//---------------------------------------------------------------------
	HRESULT D3D9RenderSystem::__SetRenderState(D3DRENDERSTATETYPE state, DWORD value)
	{
		HRESULT hr;
		DWORD oldVal;

		if ( FAILED( hr = getActiveD3D9Device()->GetRenderState(state, &oldVal) ) )
			return hr;
		if ( oldVal == value )
			return D3D_OK;
		else
			return getActiveD3D9Device()->SetRenderState(state, value);
	}
	//---------------------------------------------------------------------
	HRESULT D3D9RenderSystem::__SetSamplerState(DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD value)
	{
		HRESULT hr;
		DWORD oldVal;

		if ( FAILED( hr = getActiveD3D9Device()->GetSamplerState(sampler, type, &oldVal) ) )
			return hr;
		if ( oldVal == value )
			return D3D_OK;
		else
			return getActiveD3D9Device()->SetSamplerState(sampler, type, value);
	}
	//---------------------------------------------------------------------
	HRESULT D3D9RenderSystem::__SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value)
	{
		HRESULT hr;
		DWORD oldVal;

		// can only set fixed-function texture stage state
		if (stage < 8)
		{
			if ( FAILED( hr = getActiveD3D9Device()->GetTextureStageState(stage, type, &oldVal) ) )
				return hr;
			if ( oldVal == value )
				return D3D_OK;
			else
				return getActiveD3D9Device()->SetTextureStageState(stage, type, value);
		}
		else
		{
			return D3D_OK;
		}
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setRenderTarget_internal(RenderTarget *target)
	{
		mActiveRenderTarget = target;

		HRESULT hr;


		// If this is called without going through RenderWindow::update, then 
		// the device will not have been set. Calling it twice is safe, the 
		// implementation ensures nothing happens if the same device is set twice
		if (std::find(mRenderWindows.begin(), mRenderWindows.end(), target) != mRenderWindows.end())
		{
			D3D9RenderWindow *window = static_cast<D3D9RenderWindow*>(target);
			mDeviceManager->setActiveRenderTargetDevice(window->getDevice());
			// also make sure we validate the device; if this never went 
			// through update() it won't be set
			window->_validateDevice();
		}

		// Retrieve render surfaces (up to CM_MAX_MULTIPLE_RENDER_TARGETS)
		IDirect3DSurface9* pBack[CM_MAX_MULTIPLE_RENDER_TARGETS];
		memset(pBack, 0, sizeof(pBack));
		target->getCustomAttribute( "DDBACKBUFFER", &pBack );
		if (!pBack[0])
			return;

		IDirect3DSurface9* pDepth = NULL;

		//Check if we saved a depth buffer for this target
		TargetDepthStencilMap::iterator savedTexture = mCheckedOutTextures.find(target);
		if (savedTexture != mCheckedOutTextures.end())
		{
			pDepth = savedTexture->second.surface;
		}

		if (!pDepth)
			target->getCustomAttribute( "D3DZBUFFER", &pDepth );
		if (!pDepth)
		{
			/// No depth buffer provided, use our own
			/// Request a depth stencil that is compatible with the format, multisample type and
			/// dimensions of the render target.
			D3DSURFACE_DESC srfDesc;
			if(FAILED(pBack[0]->GetDesc(&srfDesc)))
				return; // ?
			pDepth = _getDepthStencilFor(srfDesc.Format, srfDesc.MultiSampleType, srfDesc.MultiSampleQuality, srfDesc.Width, srfDesc.Height);
		}
		// Bind render targets
		UINT32 count = mCurrentCapabilities->getNumMultiRenderTargets();
		for(UINT32 x=0; x<count; ++x)
		{
			hr = getActiveD3D9Device()->SetRenderTarget(x, pBack[x]);
			if (FAILED(hr))
			{
				String msg = DXGetErrorDescription(hr);
				CM_EXCEPT(RenderingAPIException, "Failed to setRenderTarget : " + msg);
			}
		}
		hr = getActiveD3D9Device()->SetDepthStencilSurface(pDepth);
		if (FAILED(hr))
		{
			String msg = DXGetErrorDescription(hr);
			CM_EXCEPT(RenderingAPIException, "Failed to setDepthStencil : " + msg);
		}
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setViewport_internal(const Viewport& vp)
	{
		mActiveViewport = vp;

		// ok, it's different, time to set render target and viewport params
		D3DVIEWPORT9 d3dvp;
		HRESULT hr;

		// Set render target
		RenderTarget* target = vp.getTarget();
		setRenderTarget_internal(target);

		setCullingMode_internal( mCullingMode );

		// set viewport dimensions
		d3dvp.X = vp.getActualLeft();
		d3dvp.Y = vp.getActualTop();
		d3dvp.Width = vp.getActualWidth();
		d3dvp.Height = vp.getActualHeight();
		if (target->requiresTextureFlipping())
		{
			// Convert "top-left" to "bottom-left"
			d3dvp.Y = target->getHeight() - d3dvp.Height - d3dvp.Y;
		}

		// Z-values from 0.0 to 1.0 (TODO: standardise with OpenGL)
		d3dvp.MinZ = 0.0f;
		d3dvp.MaxZ = 1.0f;

		if( FAILED( hr = getActiveD3D9Device()->SetViewport( &d3dvp ) ) )
			CM_EXCEPT(RenderingAPIException, "Failed to set viewport.");

		// Set sRGB write mode
		__SetRenderState(D3DRS_SRGBWRITEENABLE, target->isHardwareGammaEnabled());
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::beginFrame_internal()
	{
		HRESULT hr;

		if( FAILED( hr = getActiveD3D9Device()->BeginScene() ) )
		{
			String msg = DXGetErrorDescription(hr);
			CM_EXCEPT(RenderingAPIException, "Error beginning frame :" + msg);
		}

		mLastVertexSourceCount = 0;

		// Clear left overs of previous viewport.
		// I.E: Viewport A can use 3 different textures and light states
		// When trying to render viewport B these settings should be cleared, otherwise 
		// graphical artifacts might occur.
 		mDeviceManager->getActiveDevice()->clearDeviceStreams();
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::endFrame_internal()
	{
		HRESULT hr;
		if( FAILED( hr = getActiveD3D9Device()->EndScene() ) )
			CM_EXCEPT(RenderingAPIException, "Error ending frame");

		mDeviceManager->destroyInactiveRenderDevices();
	}
	//---------------------------------------------------------------------
	D3D9RenderSystem::ZBufferIdentifier D3D9RenderSystem::getZBufferIdentifier(RenderTarget* rt)
	{
		// Retrieve render surfaces (up to CM_MAX_MULTIPLE_RENDER_TARGETS)
		IDirect3DSurface9* pBack[CM_MAX_MULTIPLE_RENDER_TARGETS];
		memset(pBack, 0, sizeof(pBack));
		rt->getCustomAttribute( "DDBACKBUFFER", &pBack );
		assert(pBack[0]);

		/// Request a depth stencil that is compatible with the format, multisample type and
		/// dimensions of the render target.
		D3DSURFACE_DESC srfDesc;
		HRESULT hr = pBack[0]->GetDesc(&srfDesc);
		assert(!(FAILED(hr)));
		D3DFORMAT dsfmt = _getDepthStencilFormatFor(srfDesc.Format);
		assert(dsfmt != D3DFMT_UNKNOWN);

		/// Build identifier and return
		ZBufferIdentifier zBufferIdentifier;
		zBufferIdentifier.format = dsfmt;
		zBufferIdentifier.multisampleType = srfDesc.MultiSampleType;
		zBufferIdentifier.device = getActiveD3D9Device();
		return zBufferIdentifier;
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setVertexDeclaration_internal(VertexDeclarationPtr decl)
	{
		HRESULT hr;

		std::shared_ptr<D3D9VertexDeclaration> d3ddecl = 
			std::static_pointer_cast<D3D9VertexDeclaration>(decl);

		if (FAILED(hr = getActiveD3D9Device()->SetVertexDeclaration(d3ddecl->getD3DVertexDeclaration())))
		{
			CM_EXCEPT(RenderingAPIException, "Unable to set D3D9 vertex declaration");
		}

	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setVertexBufferBinding_internal(VertexBufferBinding* binding)
	{
		HRESULT hr;

		// TODO: attempt to detect duplicates
		const VertexBufferBinding::VertexBufferBindingMap& binds = binding->getBindings();
		VertexBufferBinding::VertexBufferBindingMap::const_iterator i, iend;
		size_t source = 0;
		iend = binds.end();
		for (i = binds.begin(); i != iend; ++i, ++source)
		{
			// Unbind gap sources
			for ( ; source < i->first; ++source)
			{
				hr = getActiveD3D9Device()->SetStreamSource(static_cast<UINT>(source), NULL, 0, 0);
				if (FAILED(hr))
				{
					CM_EXCEPT(RenderingAPIException, "Unable to reset unused D3D9 stream source");
				}
			}

			D3D9HardwareVertexBuffer* d3d9buf = 
				static_cast<D3D9HardwareVertexBuffer*>(i->second.get());
			hr = getActiveD3D9Device()->SetStreamSource(
				static_cast<UINT>(source),
				d3d9buf->getD3D9VertexBuffer(),
				0, // no stream offset, this is handled in _render instead
				static_cast<UINT>(d3d9buf->getVertexSize()) // stride
				);
			if (FAILED(hr))
			{
				CM_EXCEPT(RenderingAPIException, "Unable to set D3D9 stream source for buffer binding");
			}


		}

		// Unbind any unused sources
		for (size_t unused = source; unused < mLastVertexSourceCount; ++unused)
		{

			hr = getActiveD3D9Device()->SetStreamSource(static_cast<UINT>(unused), NULL, 0, 0);
			if (FAILED(hr))
			{
				CM_EXCEPT(RenderingAPIException, "Unable to reset unused D3D9 stream source");
			}

		}
		mLastVertexSourceCount = source;

	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::render_internal(const RenderOperation& op)
	{
		// Exit immediately if there is nothing to render
		// This caused a problem on FireGL 8800
		if (op.vertexData->vertexCount == 0)
			return;

		// Call super class
		RenderSystem::render_internal(op);

		// To think about: possibly remove setVertexDeclaration and 
		// setVertexBufferBinding from RenderSystem since the sequence is
		// a bit too D3D9-specific?
		setVertexDeclaration_internal(op.vertexData->vertexDeclaration);
		setVertexBufferBinding_internal(op.vertexData->vertexBufferBinding);

		// Determine rendering operation
		D3DPRIMITIVETYPE primType = D3DPT_TRIANGLELIST;
		DWORD primCount = 0;
		switch( op.operationType )
		{
		case RenderOperation::OT_POINT_LIST:
			primType = D3DPT_POINTLIST;
			primCount = static_cast<DWORD>(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount);
			break;

		case RenderOperation::OT_LINE_LIST:
			primType = D3DPT_LINELIST;
			primCount = static_cast<DWORD>(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) / 2;
			break;

		case RenderOperation::OT_LINE_STRIP:
			primType = D3DPT_LINESTRIP;
			primCount = static_cast<DWORD>(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) - 1;
			break;

		case RenderOperation::OT_TRIANGLE_LIST:
			primType = D3DPT_TRIANGLELIST;
			primCount = static_cast<DWORD>(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) / 3;
			break;

		case RenderOperation::OT_TRIANGLE_STRIP:
			primType = D3DPT_TRIANGLESTRIP;
			primCount = static_cast<DWORD>(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) - 2;
			break;

		case RenderOperation::OT_TRIANGLE_FAN:
			primType = D3DPT_TRIANGLEFAN;
			primCount = static_cast<DWORD>(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) - 2;
			break;
		}

		if (!primCount)
			return;

		// Issue the op
		HRESULT hr;
		if( op.useIndexes )
		{
			D3D9HardwareIndexBuffer* d3dIdxBuf = 
				static_cast<D3D9HardwareIndexBuffer*>(op.indexData->indexBuffer.get());
			hr = getActiveD3D9Device()->SetIndices( d3dIdxBuf->getD3DIndexBuffer() );
			if (FAILED(hr))
			{
				CM_EXCEPT(RenderingAPIException, "Failed to set index buffer");
			}

			// do indexed draw operation
			hr = getActiveD3D9Device()->DrawIndexedPrimitive(
				primType, 
				static_cast<INT>(op.vertexData->vertexStart), 
				0, // Min vertex index - assume we can go right down to 0 
				static_cast<UINT>(op.vertexData->vertexCount), 
				static_cast<UINT>(op.indexData->indexStart), 
				static_cast<UINT>(primCount)
				);
		}
		else
		{
			// Unindexed, a little simpler!
			hr = getActiveD3D9Device()->DrawPrimitive(
				primType, 
				static_cast<UINT>(op.vertexData->vertexStart), 
				static_cast<UINT>(primCount)
				); 
		} 

		if( FAILED( hr ) )
		{
			String msg = DXGetErrorDescription(hr);
			CM_EXCEPT(RenderingAPIException, "Failed to DrawPrimitive : " + msg);
		}

	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setClipPlanesImpl(const PlaneList& clipPlanes)
	{
		size_t i;
		size_t numClipPlanes;
		D3DXPLANE dx9ClipPlane;
		DWORD mask = 0;
		HRESULT hr;

		numClipPlanes = clipPlanes.size();
		for (i = 0; i < numClipPlanes; ++i)
		{
			const Plane& plane = clipPlanes[i];

			dx9ClipPlane.a = plane.normal.x;
			dx9ClipPlane.b = plane.normal.y;
			dx9ClipPlane.c = plane.normal.z;
			dx9ClipPlane.d = plane.d;

			if (mVertexProgramBound)
			{
				// programmable clips in clip space (ugh)
				// must transform worldspace planes by view/proj
				D3DXMATRIX xform;
				D3DXMatrixMultiply(&xform, &mDxViewMat, &mDxProjMat);
				D3DXMatrixInverse(&xform, NULL, &xform);
				D3DXMatrixTranspose(&xform, &xform);
				D3DXPlaneTransform(&dx9ClipPlane, &dx9ClipPlane, &xform);
			}

			hr = getActiveD3D9Device()->SetClipPlane(static_cast<DWORD>(i), dx9ClipPlane);
			if (FAILED(hr))
			{
				CM_EXCEPT(RenderingAPIException, "Unable to set clip plane");
			}

			mask |= (1 << i);
		}

		hr = __SetRenderState(D3DRS_CLIPPLANEENABLE, mask);
		if (FAILED(hr))
		{
			CM_EXCEPT(RenderingAPIException, "Unable to set render state for clip planes");
		}
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setScissorTest_internal(bool enabled, size_t left, size_t top, size_t right,
		size_t bottom)
	{
		HRESULT hr;
		if (enabled)
		{
			if (FAILED(hr = __SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE)))
			{
				CM_EXCEPT(RenderingAPIException, "Unable to enable scissor rendering state; " + getErrorDescription_internal(hr));
			}
			RECT rect;
			rect.left = static_cast<LONG>(left);
			rect.top = static_cast<LONG>(top);
			rect.bottom = static_cast<LONG>(bottom);
			rect.right = static_cast<LONG>(right);
			if (FAILED(hr = getActiveD3D9Device()->SetScissorRect(&rect)))
			{
				CM_EXCEPT(RenderingAPIException, "Unable to set scissor rectangle; " + getErrorDescription_internal(hr));
			}
		}
		else
		{
			if (FAILED(hr = __SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE)))
			{
				CM_EXCEPT(RenderingAPIException, "Unable to disable scissor rendering state; " + getErrorDescription_internal(hr));
			}
		}
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::clearFrameBuffer_internal(unsigned int buffers, 
		const Color& colour, float depth, unsigned short stencil)
	{
		DWORD flags = 0;
		if (buffers & FBT_COLOUR)
		{
			flags |= D3DCLEAR_TARGET;
		}
		if (buffers & FBT_DEPTH)
		{
			flags |= D3DCLEAR_ZBUFFER;
		}
		// Only try to clear the stencil buffer if supported
		if (buffers & FBT_STENCIL && mCurrentCapabilities->hasCapability(RSC_HWSTENCIL))
		{
			flags |= D3DCLEAR_STENCIL;
		}
		HRESULT hr;
		if( FAILED( hr = getActiveD3D9Device()->Clear( 
			0, 
			NULL, 
			flags,
			colour.getAsARGB(), 
			depth, 
			stencil ) ) )
		{
			String msg = DXGetErrorDescription(hr);
			CM_EXCEPT(RenderingAPIException, "Error clearing frame buffer : " + msg);
		}
	}
	// ------------------------------------------------------------------
	void D3D9RenderSystem::setClipPlane (UINT16 index, float A, float B, float C, float D)
	{
		float plane[4] = { A, B, C, D };
		getActiveD3D9Device()->SetClipPlane (index, plane);
	}

	// ------------------------------------------------------------------
	void D3D9RenderSystem::enableClipPlane (UINT16 index, bool enable)
	{
		DWORD prev;
		getActiveD3D9Device()->GetRenderState(D3DRS_CLIPPLANEENABLE, &prev);
		__SetRenderState(D3DRS_CLIPPLANEENABLE, enable?
			(prev | (1 << index)) : (prev & ~(1 << index)));
	}
	//---------------------------------------------------------------------
	float D3D9RenderSystem::getHorizontalTexelOffset()
	{
		// D3D considers the origin to be in the center of a pixel
		return -0.5f;
	}
	//---------------------------------------------------------------------
	float D3D9RenderSystem::getVerticalTexelOffset()
	{
		// D3D considers the origin to be in the center of a pixel
		return -0.5f;
	}
	//---------------------------------------------------------------------
	float D3D9RenderSystem::getMinimumDepthInputValue()
	{
		// Range [0.0f, 1.0f]
		return 0.0f;
	}
	//---------------------------------------------------------------------
	float D3D9RenderSystem::getMaximumDepthInputValue()
	{
		// Range [0.0f, 1.0f]
		// D3D inverts even identity view matrices, so maximum INPUT is -1.0
		return -1.0f;
	}
	//---------------------------------------------------------------------
	IDirect3D9*	D3D9RenderSystem::getDirect3D9()
	{
		IDirect3D9* pDirect3D9 = msD3D9RenderSystem->mpD3D;

		if (pDirect3D9 == NULL)
		{
			CM_EXCEPT(InvalidParametersException, "Direct3D9 interface is NULL !!!");
		}

		return pDirect3D9;
	}

	//---------------------------------------------------------------------
	UINT D3D9RenderSystem::getResourceCreationDeviceCount()
	{
		D3D9ResourceCreationPolicy creationPolicy = msD3D9RenderSystem->mResourceManager->getCreationPolicy();

		if (creationPolicy == RCP_CREATE_ON_ACTIVE_DEVICE)
		{
			return 1;
		}
		else if (creationPolicy == RCP_CREATE_ON_ALL_DEVICES)
		{
			return msD3D9RenderSystem->mDeviceManager->getDeviceCount();
		}

		CM_EXCEPT(InvalidParametersException, "Invalid resource creation policy !!!" );

		return 0;
	}

	//---------------------------------------------------------------------
	IDirect3DDevice9* D3D9RenderSystem::getResourceCreationDevice(UINT index)
	{
		D3D9ResourceCreationPolicy creationPolicy = msD3D9RenderSystem->mResourceManager->getCreationPolicy();
		IDirect3DDevice9* d3d9Device = NULL;

		if (creationPolicy == RCP_CREATE_ON_ACTIVE_DEVICE)
		{
			d3d9Device = msD3D9RenderSystem->getActiveD3D9Device();
		}
		else if (creationPolicy == RCP_CREATE_ON_ALL_DEVICES) 
		{
			d3d9Device = msD3D9RenderSystem->mDeviceManager->getDevice(index)->getD3D9Device();
		}
		else
		{
			CM_EXCEPT(InvalidParametersException, "Invalid resource creation policy !!!" );
		}

		return d3d9Device;
	}

	//---------------------------------------------------------------------
	IDirect3DDevice9* D3D9RenderSystem::getActiveD3D9Device()
	{	
		D3D9Device* activeDevice = msD3D9RenderSystem->mDeviceManager->getActiveDevice();
		IDirect3DDevice9* d3d9Device;

		d3d9Device = activeDevice->getD3D9Device();

		if (d3d9Device == NULL)
		{
			CM_EXCEPT(InvalidParametersException, "Current d3d9 device is NULL !!!" );
		}

		return d3d9Device;
	}	

	//---------------------------------------------------------------------
	// Formats to try, in decreasing order of preference
	D3DFORMAT ddDepthStencilFormats[]={
		D3DFMT_D24FS8,
		D3DFMT_D24S8,
		D3DFMT_D24X4S4,
		D3DFMT_D24X8,
		D3DFMT_D15S1,
		D3DFMT_D16,
		D3DFMT_D32
	};
#define NDSFORMATS (sizeof(ddDepthStencilFormats)/sizeof(D3DFORMAT))

	D3DFORMAT D3D9RenderSystem::_getDepthStencilFormatFor(D3DFORMAT fmt)
	{
		/// Check if result is cached
		DepthStencilHash::iterator i = mDepthStencilHash.find((unsigned int)fmt);
		if(i != mDepthStencilHash.end())
			return i->second;
		/// If not, probe with CheckDepthStencilMatch
		D3DFORMAT dsfmt = D3DFMT_UNKNOWN;

		/// Get description of primary render target
		D3D9Device* activeDevice = mDeviceManager->getActiveDevice();
		IDirect3DSurface9* mSurface = activeDevice->getPrimaryWindow()->getRenderSurface();
		D3DSURFACE_DESC srfDesc;

		if(!FAILED(mSurface->GetDesc(&srfDesc)))
		{
			/// Probe all depth stencil formats
			/// Break on first one that matches
			for(size_t x=0; x<NDSFORMATS; ++x)
			{
				// Verify that the depth format exists
				if (mpD3D->CheckDeviceFormat(
					activeDevice->getAdapterNumber(),
					activeDevice->getDeviceType(),
					srfDesc.Format,
					D3DUSAGE_DEPTHSTENCIL,
					D3DRTYPE_SURFACE,
					ddDepthStencilFormats[x]) != D3D_OK)
				{
					continue;
				}
				// Verify that the depth format is compatible
				if(mpD3D->CheckDepthStencilMatch(
					activeDevice->getAdapterNumber(),
					activeDevice->getDeviceType(), 
					srfDesc.Format,
					fmt, ddDepthStencilFormats[x]) == D3D_OK)
				{
					dsfmt = ddDepthStencilFormats[x];
					break;
				}
			}
		}
		/// Cache result
		mDepthStencilHash[(unsigned int)fmt] = dsfmt;
		return dsfmt;
	}
	IDirect3DSurface9* D3D9RenderSystem::_getDepthStencilFor(D3DFORMAT fmt, 
		D3DMULTISAMPLE_TYPE multisample, DWORD multisample_quality, size_t width, size_t height)
	{
		D3DFORMAT dsfmt = _getDepthStencilFormatFor(fmt);
		if(dsfmt == D3DFMT_UNKNOWN)
			return 0;
		IDirect3DSurface9 *surface = 0;

		/// Check if result is cached
		ZBufferIdentifier zBufferIdentifer;


		zBufferIdentifer.format = dsfmt;
		zBufferIdentifer.multisampleType = multisample;
		zBufferIdentifer.device = getActiveD3D9Device();

		ZBufferRefQueue& zBuffers = mZBufferHash[zBufferIdentifer];
		
		if(!zBuffers.empty())
		{
			const ZBufferRef& zBuffer = zBuffers.front();
			/// Check if size is larger or equal
			if(zBuffer.width >= width && zBuffer.height >= height)
			{
				surface = zBuffer.surface;
			} 
			else
			{
				/// If not, destroy current buffer
				zBuffer.surface->Release();
				zBuffers.pop_front();
			}
		}
		if(!surface)
		{
			/// If not, create the depthstencil surface
			HRESULT hr = getActiveD3D9Device()->CreateDepthStencilSurface( 
				static_cast<UINT>(width), 
				static_cast<UINT>(height), 
				dsfmt, 
				multisample, 
				multisample_quality, 
				TRUE,  // discard true or false?
				&surface, 
				NULL);
			if(FAILED(hr))
			{
				String msg = DXGetErrorDescription(hr);
				CM_EXCEPT(InvalidParametersException, "Error CreateDepthStencilSurface : " + msg);
			}
			/// And cache it
			ZBufferRef zb;
			zb.surface = surface;
			zb.width = width;
			zb.height = height;
			zBuffers.push_front(zb);
		}
		return surface;
	}

	//---------------------------------------------------------------------
	void D3D9RenderSystem::_cleanupDepthStencils(IDirect3DDevice9* d3d9Device)
	{
		for(ZBufferHash::iterator i = mZBufferHash.begin(); i != mZBufferHash.end();)
		{
			/// Release buffer
			if (i->first.device == d3d9Device)
			{
				while (!i->second.empty())
				{
					IDirect3DSurface9* surface = i->second.front().surface;
					surface->Release();
					i->second.pop_front();
				}
				ZBufferHash::iterator deadi = i++;
				mZBufferHash.erase(deadi);
			}			
			else
			{
				++i;
			}
		}		
	}
	//---------------------------------------------------------------------
	D3D9ResourceManager* D3D9RenderSystem::getResourceManager()
	{
		return msD3D9RenderSystem->mResourceManager;
	}

	//---------------------------------------------------------------------
	D3D9DeviceManager* D3D9RenderSystem::getDeviceManager()
	{
		return msD3D9RenderSystem->mDeviceManager;
	}

	//---------------------------------------------------------------------
	RenderSystemCapabilities* D3D9RenderSystem::createRenderSystemCapabilities() const
	{
		return mCurrentCapabilities;
	}

	//---------------------------------------------------------------------
	void D3D9RenderSystem::notifyOnDeviceLost(D3D9Device* device)
	{	

	}

	//---------------------------------------------------------------------
	void D3D9RenderSystem::notifyOnDeviceReset(D3D9Device* device)
	{		
		// Reset state attributes.	
		mVertexProgramBound = false;
		mFragmentProgramBound = false;
		mLastVertexSourceCount = 0;
		
		// Restore previous active device.

		// Invalidate active view port.
		mActiveViewport = Viewport();

		// Reset the texture stages, they will need to be rebound
		for (size_t i = 0; i < CM_MAX_TEXTURE_LAYERS; ++i)
			setTexture_internal(i, false, TexturePtr());
	}
	
	//---------------------------------------------------------------------
	bool D3D9RenderSystem::ZBufferIdentifierComparator::operator()( const ZBufferIdentifier& z0, const ZBufferIdentifier& z1 ) const
	{
		if (z0.device < z1.device)
			return true;

		if (z0.device == z1.device)
		{
			if (z0.format < z1.format)
				return true;

			if (z0.format == z1.format)
			{
				if (z0.multisampleType < z1.multisampleType)
					return true;
			}
		}

		return false;
	}

	//---------------------------------------------------------------------
	void D3D9RenderSystem::determineFSAASettings(IDirect3DDevice9* d3d9Device,
		size_t fsaa, const String& fsaaHint, D3DFORMAT d3dPixelFormat, 
		bool fullScreen, D3DMULTISAMPLE_TYPE *outMultisampleType, DWORD *outMultisampleQuality)
	{
		bool ok = false;
		bool qualityHint = fsaaHint.find("Quality") != String::npos;
		size_t origFSAA = fsaa;

		D3D9DriverList* driverList = getDirect3DDrivers();
		D3D9Driver* deviceDriver = mActiveD3DDriver;
		D3D9Device* device = mDeviceManager->getDeviceFromD3D9Device(d3d9Device);

		for (UINT32 i = 0; i < driverList->count(); ++i)
		{
			D3D9Driver* currDriver = driverList->item(i);

			if (currDriver->getAdapterNumber() == device->getAdapterNumber())
			{
				deviceDriver = currDriver;
				break;
			}
		}

		bool tryCSAA = false;
		// NVIDIA, prefer CSAA if available for 8+
		// it would be tempting to use getCapabilities()->getVendor() == GPU_NVIDIA but
		// if this is the first window, caps will not be initialised yet
		if (deviceDriver->getAdapterIdentifier().VendorId == 0x10DE && 
			fsaa >= 8)
		{
			tryCSAA	 = true;
		}

		while (!ok)
		{
			// Deal with special cases
			if (tryCSAA)
			{
				// see http://developer.nvidia.com/object/coverage-sampled-aa.html
				switch(fsaa)
				{
				case 8:
					if (qualityHint)
					{
						*outMultisampleType = D3DMULTISAMPLE_8_SAMPLES;
						*outMultisampleQuality = 0;
					}
					else
					{
						*outMultisampleType = D3DMULTISAMPLE_4_SAMPLES;
						*outMultisampleQuality = 2;
					}
					break;
				case 16:
					if (qualityHint)
					{
						*outMultisampleType = D3DMULTISAMPLE_8_SAMPLES;
						*outMultisampleQuality = 2;
					}
					else
					{
						*outMultisampleType = D3DMULTISAMPLE_4_SAMPLES;
						*outMultisampleQuality = 4;
					}
					break;
				}
			}
			else // !CSAA
			{
				*outMultisampleType = (D3DMULTISAMPLE_TYPE)fsaa;
				*outMultisampleQuality = 0;
			}


			HRESULT hr;
			DWORD outQuality;
			hr = mpD3D->CheckDeviceMultiSampleType( 
				deviceDriver->getAdapterNumber(), 
				D3DDEVTYPE_HAL, 
				d3dPixelFormat, 
				fullScreen, 
				*outMultisampleType, 
				&outQuality);

			if (SUCCEEDED(hr) && 
				(!tryCSAA || outQuality > *outMultisampleQuality))
			{
				ok = true;
			}
			else
			{
				// downgrade
				if (tryCSAA && fsaa == 8)
				{
					// for CSAA, we'll try downgrading with quality mode at all samples.
					// then try without quality, then drop CSAA
					if (qualityHint)
					{
						// drop quality first
						qualityHint = false;
					}
					else
					{
						// drop CSAA entirely 
						tryCSAA = false;
					}
					// return to original requested samples
					fsaa = origFSAA;
				}
				else
				{
					// drop samples
					--fsaa;

					if (fsaa == 1)
					{
						// ran out of options, no FSAA
						fsaa = 0;
						ok = true;
					}
				}
			}

		} // while !ok
	}

	/************************************************************************/
	/* 							INTERNAL CALLBACKS                     		*/
	/************************************************************************/
	void D3D9RenderSystem::startUp_internal()
	{
		// Create the resource manager.
		mResourceManager = new D3D9ResourceManager();

		// Create our Direct3D object
		if( NULL == (mpD3D = Direct3DCreate9(D3D_SDK_VERSION)) )
			CM_EXCEPT(InternalErrorException, "Failed to create Direct3D9 object");

		// set config options defaults
		initConfigOptions();

		// fsaa options
		mFSAAHint = "";
		mFSAASamples = 0;

		// set stages desc. to defaults
		for (size_t n = 0; n < CM_MAX_TEXTURE_LAYERS; n++)
		{
			mTexStageDesc[n].autoTexCoordType = TEXCALC_NONE;
			mTexStageDesc[n].coordIndex = 0;
			mTexStageDesc[n].texType = D3D9Mappings::D3D_TEX_TYPE_NORMAL;
			mTexStageDesc[n].pTex = 0;
			mTexStageDesc[n].pVertexTex = 0;
		}

		mLastVertexSourceCount = 0;

		mCurrentLights.clear();	

		RenderWindow* autoWindow = NULL;

		// Init using current settings
		mActiveD3DDriver = NULL;
		ConfigOptionMap::iterator opt = mOptions.find( "Rendering Device" );
		for( UINT32 j=0; j < getDirect3DDrivers()->count(); j++ )
		{
			if( getDirect3DDrivers()->item(j)->DriverDescription() == opt->second.currentValue )
			{
				mActiveD3DDriver = getDirect3DDrivers()->item(j);
				break;
			}
		}

		if( !mActiveD3DDriver )
			CM_EXCEPT(InvalidParametersException, "Problems finding requested Direct3D driver!" );

		// get driver version
		mDriverVersion.major = HIWORD(mActiveD3DDriver->getAdapterIdentifier().DriverVersion.HighPart);
		mDriverVersion.minor = LOWORD(mActiveD3DDriver->getAdapterIdentifier().DriverVersion.HighPart);
		mDriverVersion.release = HIWORD(mActiveD3DDriver->getAdapterIdentifier().DriverVersion.LowPart);
		mDriverVersion.build = LOWORD(mActiveD3DDriver->getAdapterIdentifier().DriverVersion.LowPart);

		// Create the device manager.
		mDeviceManager = new D3D9DeviceManager();

		// Create the texture manager for use by others		
		TextureManager::startUp(new D3D9TextureManager());

		// Also create hardware buffer manager		
		HardwareBufferManager::startUp(new D3D9HardwareBufferManager());

		// Create the GPU program manager		
		GpuProgramManager::startUp(new D3D9GpuProgramManager());

		// Create & register HLSL factory		
		mHLSLProgramFactory = new D3D9HLSLProgramFactory();

		// Create & register Cg factory		
		mCgProgramFactory = new CgProgramFactory();

		// call superclass method
		RenderSystem::startUp_internal();
	}

	void D3D9RenderSystem::createRenderWindow_internal(const String &name, 
		unsigned int width, unsigned int height, bool fullScreen,
		const NameValuePairList& miscParams, AsyncOp& asyncOp)
	{		
		String msg;

		// Make sure we don't already have a render target of the 
		// same name as the one supplied
		if( mRenderTargets.find( name ) != mRenderTargets.end() )
		{
			msg = "A render target of the same name '" + name + "' already "
				"exists.  You cannot create a new window with this name.";
			CM_EXCEPT(InternalErrorException, msg);
		}

		D3D9RenderWindow* renderWindow = new D3D9RenderWindow(mhInstance);

		renderWindow->create(name, width, height, fullScreen, &miscParams);

		mResourceManager->lockDeviceAccess();

		try
		{
			mDeviceManager->linkRenderWindow(renderWindow);
		}
		catch (const CamelotEngine::RenderingAPIException&)
		{
			// after catching the exception, clean up
			mResourceManager->unlockDeviceAccess();
			renderWindow->destroy();

			// re-throw
			throw;
		}

		mResourceManager->unlockDeviceAccess();

		mRenderWindows.push_back(renderWindow);		

		updateRenderSystemCapabilities(renderWindow);

		attachRenderTarget_internal( *renderWindow );

		asyncOp.completeOperation(static_cast<RenderWindow*>(renderWindow));
	}	

	void D3D9RenderSystem::bindGpuProgram_internal(GpuProgramRef prg)
	{
		GpuProgram* bindingPrg = prg->_getBindingDelegate();

		HRESULT hr;
		switch (bindingPrg->getType())
		{
		case GPT_VERTEX_PROGRAM:
			hr = getActiveD3D9Device()->SetVertexShader(
				static_cast<D3D9GpuVertexProgram*>(bindingPrg)->getVertexShader());
			if (FAILED(hr))
			{
				CM_EXCEPT(RenderingAPIException, "Error calling SetVertexShader");
			}
			break;
		case GPT_FRAGMENT_PROGRAM:
			hr = getActiveD3D9Device()->SetPixelShader(
				static_cast<D3D9GpuFragmentProgram*>(bindingPrg)->getPixelShader());
			if (FAILED(hr))
			{
				CM_EXCEPT(RenderingAPIException, "Error calling SetPixelShader");
			}
			break;
		};

		// Make sure texcoord index is equal to stage value, As SDK Doc suggests:
		// "When rendering using vertex shaders, each stage's texture coordinate index must be set to its default value."
		// This solves such an errors when working with the Debug runtime -
		// "Direct3D9: (ERROR) :Stage 1 - Texture coordinate index in the stage must be equal to the stage index when programmable vertex pipeline is used".
		for (unsigned int nStage=0; nStage < 8; ++nStage)
			__SetTextureStageState(nStage, D3DTSS_TEXCOORDINDEX, nStage);

		RenderSystem::bindGpuProgram_internal(prg);

	}

	void D3D9RenderSystem::unbindGpuProgram_internal(GpuProgramType gptype)
	{
		HRESULT hr;
		switch(gptype)
		{
		case GPT_VERTEX_PROGRAM:
			mActiveVertexGpuProgramParameters = nullptr;
			hr = getActiveD3D9Device()->SetVertexShader(NULL);
			if (FAILED(hr))
			{
				CM_EXCEPT(RenderingAPIException, "Error resetting SetVertexShader to NULL");
			}
			break;
		case GPT_FRAGMENT_PROGRAM:
			mActiveFragmentGpuProgramParameters = nullptr;
			hr = getActiveD3D9Device()->SetPixelShader(NULL);
			if (FAILED(hr))
			{
				CM_EXCEPT(RenderingAPIException, "Error resetting SetPixelShader to NULL");
			}
			break;
		};

		RenderSystem::unbindGpuProgram_internal(gptype);
	}

	void D3D9RenderSystem::bindGpuProgramParameters_internal(GpuProgramType gptype, 
		GpuProgramParametersSharedPtr params, UINT16 variability)
	{
		HRESULT hr;
		GpuLogicalBufferStructPtr floatLogical = params->getFloatLogicalBufferStruct();
		GpuLogicalBufferStructPtr intLogical = params->getIntLogicalBufferStruct();

		GpuLogicalBufferStructPtr samplerLogical = params->getSamplerLogicalBufferStruct();

		// Set texture sampler
		{
			CM_LOCK_MUTEX(samplerLogical->mutex)

				for (GpuLogicalIndexUseMap::const_iterator i = samplerLogical->map.begin();
					i != samplerLogical->map.end(); ++i)
				{
					if (i->second.variability & variability)
					{
						size_t logicalIndex = i->first;
						TextureRef texture = params->getTexture(i->second.physicalIndex);

						const SamplerState& samplerState = params->getSamplerState(i->second.physicalIndex);

						setTextureUnitSettings_internal(logicalIndex, texture.getInternalPtr(), samplerState);
					}
				}
		}

		switch(gptype)
		{
		case GPT_VERTEX_PROGRAM:
			mActiveVertexGpuProgramParameters = params;
			{
				CM_LOCK_MUTEX(floatLogical->mutex)

					for (GpuLogicalIndexUseMap::const_iterator i = floatLogical->map.begin();
						i != floatLogical->map.end(); ++i)
					{
						if (i->second.variability & variability)
						{
							size_t logicalIndex = i->first;
							const float* pFloat = params->getFloatPointer(i->second.physicalIndex);
							size_t slotCount = i->second.currentSize / 4;
							assert (i->second.currentSize % 4 == 0 && "Should not have any "
								"elements less than 4 wide for D3D9");

							if (FAILED(hr = getActiveD3D9Device()->SetVertexShaderConstantF( // TODO Low priority. Binding parameters 1 by 1 is slow. It would be better to keep them in a sequential
								(UINT)logicalIndex, pFloat, (UINT)slotCount)))               // buffer and then only call this method once
							{
								CM_EXCEPT(RenderingAPIException, "Unable to upload vertex shader float parameters");
							}
						}

					}

			}
			// bind ints
			{
				CM_LOCK_MUTEX(intLogical->mutex)

					for (GpuLogicalIndexUseMap::const_iterator i = intLogical->map.begin();
						i != intLogical->map.end(); ++i)
					{
						if (i->second.variability & variability)
						{
							size_t logicalIndex = i->first;
							const int* pInt = params->getIntPointer(i->second.physicalIndex);
							size_t slotCount = i->second.currentSize / 4;
							assert (i->second.currentSize % 4 == 0 && "Should not have any "
								"elements less than 4 wide for D3D9");

							if (FAILED(hr = getActiveD3D9Device()->SetVertexShaderConstantI(
								static_cast<UINT>(logicalIndex), pInt, static_cast<UINT>(slotCount))))
							{
								CM_EXCEPT(RenderingAPIException, "Unable to upload vertex shader int parameters");
							}
						}
					}

			}

			break;
		case GPT_FRAGMENT_PROGRAM:
			mActiveFragmentGpuProgramParameters = params;
			{
				CM_LOCK_MUTEX(floatLogical->mutex)

					for (GpuLogicalIndexUseMap::const_iterator i = floatLogical->map.begin();
						i != floatLogical->map.end(); ++i)
					{
						if (i->second.variability & variability)
						{
							size_t logicalIndex = i->first;
							const float* pFloat = params->getFloatPointer(i->second.physicalIndex);
							size_t slotCount = i->second.currentSize / 4;
							assert (i->second.currentSize % 4 == 0 && "Should not have any "
								"elements less than 4 wide for D3D9");

							if (FAILED(hr = getActiveD3D9Device()->SetPixelShaderConstantF(
								static_cast<UINT>(logicalIndex), pFloat, static_cast<UINT>(slotCount))))
							{
								CM_EXCEPT(RenderingAPIException, "Unable to upload pixel shader float parameters");
							}
						}
					}

			}
			// bind ints
			{
				CM_LOCK_MUTEX(intLogical->mutex)

					for (GpuLogicalIndexUseMap::const_iterator i = intLogical->map.begin();
						i != intLogical->map.end(); ++i)
					{
						if (i->second.variability & variability)
						{
							size_t logicalIndex = i->first;
							const int* pInt = params->getIntPointer(i->second.physicalIndex);
							size_t slotCount = i->second.currentSize / 4;
							assert (i->second.currentSize % 4 == 0 && "Should not have any "
								"elements less than 4 wide for D3D9");

							if (FAILED(hr = getActiveD3D9Device()->SetPixelShaderConstantI(
								static_cast<UINT>(logicalIndex), pInt, static_cast<UINT>(slotCount))))
							{
								CM_EXCEPT(RenderingAPIException, "Unable to upload pixel shader int parameters");
							}
						}

					}

			}
			break;
		};
	}
}
