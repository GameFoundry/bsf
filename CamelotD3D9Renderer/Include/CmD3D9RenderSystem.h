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
#ifndef __D3D9RENDERSYSTEM_H__
#define __D3D9RENDERSYSTEM_H__

#include "CmD3D9Prerequisites.h"
#include "CmString.h"
#include "CmConfigOptionMap.h"
#include "CmRenderSystem.h"
#include "CmRenderSystemCapabilities.h"
#include "CmD3D9Mappings.h"
#include "CmCgProgramFactory.h"

namespace CamelotEngine 
{
	class D3D9DriverList;
	class D3D9Driver;
	class D3D9Device;
	class D3D9DeviceManager;
	class D3D9ResourceManager;

	/**
	Implementation of DirectX9 as a rendering system.
	*/
	class CM_D3D9_EXPORT D3D9RenderSystem : public RenderSystem
	{
	private:
		/// Direct3D
		IDirect3D9*	 mpD3D;		
		// Stored options
		ConfigOptionMap mOptions;
		size_t mFSAASamples;
		String mFSAAHint;

		/// instance
		HINSTANCE mhInstance;

		/// List of D3D drivers installed (video cards)
		D3D9DriverList* mDriverList;
		/// Currently active driver
		D3D9Driver* mActiveD3DDriver;
		/// NVPerfHUD allowed?
		bool mUseNVPerfHUD;
		/// Per-stage constant support? (not in main caps since D3D specific & minor)
		bool mPerStageConstantSupport;
		/// Fast singleton access.
		static D3D9RenderSystem* msD3D9RenderSystem;

		/// structure holding texture unit settings for every stage
		struct sD3DTextureStageDesc
		{
			/// the type of the texture
			D3D9Mappings::eD3DTexType texType;
			/// which texCoordIndex to use
			size_t coordIndex;
			/// type of auto tex. calc. used
			TexCoordCalcMethod autoTexCoordType;
			/// texture 
			IDirect3DBaseTexture9 *pTex;
			/// vertex texture 
			IDirect3DBaseTexture9 *pVertexTex;
		} mTexStageDesc[CM_MAX_TEXTURE_LAYERS];

		D3D9DriverList* getDirect3DDrivers();
		void refreshD3DSettings();
        void refreshFSAAOptions();
				
		// state management methods, very primitive !!!
		HRESULT __SetRenderState(D3DRENDERSTATETYPE state, DWORD value);
		HRESULT __SetSamplerState(DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD value);
		HRESULT __SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value);

		HRESULT __SetFloatRenderState(D3DRENDERSTATETYPE state, float value)
		{
#if OGRE_DOUBLE_PRECISION == 1
			float temp = static_cast<float>(value);
			return __SetRenderState(state, *((LPDWORD)(&temp)));
#else
			return __SetRenderState(state, *((LPDWORD)(&value)));
#endif
		}

		/// return anisotropy level
		DWORD _getCurrentAnisotropy(size_t unit);
		/// check if a FSAA is supported
		bool _checkMultiSampleQuality(D3DMULTISAMPLE_TYPE type, DWORD *outQuality, D3DFORMAT format, UINT adapterNum, D3DDEVTYPE deviceType, BOOL fullScreen);
		
        D3D9HLSLProgramFactory* mHLSLProgramFactory;
		CgProgramFactory* mCgProgramFactory;
		D3D9ResourceManager* mResourceManager;
		D3D9DeviceManager* mDeviceManager;

		size_t mLastVertexSourceCount;


        /// Internal method for populating the capabilities structure
		virtual RenderSystemCapabilities* createRenderSystemCapabilities() const;
		RenderSystemCapabilities* updateRenderSystemCapabilities(D3D9RenderWindow* renderWindow);

		/** See RenderSystem definition */
		virtual void initialiseFromRenderSystemCapabilities(RenderSystemCapabilities* caps, RenderTarget* primary);


        void convertVertexShaderCaps(RenderSystemCapabilities* rsc) const;
        void convertPixelShaderCaps(RenderSystemCapabilities* rsc) const;
		bool checkVertexTextureFormats(D3D9RenderWindow* renderWindow) const;
		
        HashMap<IDirect3DDevice9*, unsigned short> mCurrentLights;
        /// Saved last view matrix
        Matrix4 mViewMatrix;

		D3DXMATRIX mDxViewMat, mDxProjMat, mDxWorldMat;
	
		typedef vector<D3D9RenderWindow*>::type D3D9RenderWindowList;
		// List of additional windows after the first (swap chains)
		D3D9RenderWindowList mRenderWindows;
		
		/** Mapping of texture format -> DepthStencil. Used as cache by _getDepthStencilFormatFor
		*/
		typedef HashMap<unsigned int, D3DFORMAT> DepthStencilHash;
		DepthStencilHash mDepthStencilHash;

		/** Mapping of depthstencil format -> depthstencil buffer
			Keep one depthstencil buffer around for every format that is used, it must be large
			enough to hold the largest rendering target.
			This is used as cache by _getDepthStencilFor.
		*/
		struct ZBufferIdentifier
		{
			IDirect3DDevice9* device;
			D3DFORMAT format;
			D3DMULTISAMPLE_TYPE multisampleType;
		};
		struct ZBufferRef
		{
			IDirect3DSurface9 *surface;
			size_t width, height;
		};
		struct ZBufferIdentifierComparator
		{
			bool operator()(const ZBufferIdentifier& z0, const ZBufferIdentifier& z1) const;
		};
		
		typedef deque<ZBufferRef>::type ZBufferRefQueue;
		typedef map<ZBufferIdentifier, ZBufferRefQueue, ZBufferIdentifierComparator>::type ZBufferHash;
		ZBufferHash mZBufferHash;		

	protected:
		// I know that's a lot of friends, but I'd rather have friend classes than exposing the needed methods
		// as public interface. 
		friend class D3D9Texture;
		friend class D3D9RenderWindow;
		friend class D3D9Device;
		friend class D3D9TextureManager;

		void setClipPlanesImpl(const PlaneList& clipPlanes);	

		/// @copydoc RenderSystem::createMultiRenderTarget
		virtual MultiRenderTarget * createMultiRenderTarget(const String & name);

		String getErrorDescription( long errorNumber ) const;

		void initConfigOptions();
		void setClipPlane (UINT16 index, float A, float B, float C, float D);
		void enableClipPlane (UINT16 index, bool enable);

		/**
			Get the matching Z-Buffer identifier for a certain render target
		*/
		ZBufferIdentifier getZBufferIdentifier(RenderTarget* rt);

		/** Check which depthStencil formats can be used with a certain pixel format,
			and return the best suited.
		*/
		D3DFORMAT getDepthStencilFormatFor(D3DFORMAT fmt);

		/** Get a depth stencil surface that is compatible with an internal pixel format and
			multisample type.
			@returns A directx surface, or 0 if there is no compatible depthstencil possible.
		*/
		IDirect3DSurface9* getDepthStencilFor(D3DFORMAT fmt, D3DMULTISAMPLE_TYPE multisample, DWORD multisample_quality, size_t width, size_t height);

		/** Clear all cached depth stencil surfaces
		*/
		void cleanupDepthStencils(IDirect3DDevice9* d3d9Device);

        /** Check whether or not filtering is supported for the precise texture format requested
        with the given usage options.
        */
        bool checkTextureFilteringSupported(TextureType ttype, PixelFormat format, int usage);

		/// Take in some requested FSAA settings and output supported D3D settings
		void determineFSAASettings(IDirect3DDevice9* d3d9Device, size_t fsaa, const String& fsaaHint, D3DFORMAT d3dPixelFormat, 
			bool fullScreen, D3DMULTISAMPLE_TYPE *outMultisampleType, DWORD *outMultisampleQuality);
	public:
		// constructor
		D3D9RenderSystem( HINSTANCE hInstance );
		// destructor
		~D3D9RenderSystem();

		const String& getName() const;
		void shutdown();

		void setStencilCheckEnabled_internal(bool enabled);
        void setStencilBufferParams_internal(CompareFunction func = CMPF_ALWAYS_PASS, 
            UINT32 refValue = 0, UINT32 mask = 0xFFFFFFFF, 
            StencilOperation stencilFailOp = SOP_KEEP, 
            StencilOperation depthFailOp = SOP_KEEP,
            StencilOperation passOp = SOP_KEEP, 
            bool twoSidedOperation = false);

		void startUp_internal();
		void createRenderWindow_internal(const String &name, unsigned int width, unsigned int height, 
			bool fullScreen, const NameValuePairList& miscParams, AsyncOp& asyncOp);
		void destroyRenderTarget_internal(const String& name);
		void setRenderTarget_internal(RenderTarget *target);

		void bindGpuProgram_internal(GpuProgramHandle prg);
		void unbindGpuProgram_internal(GpuProgramType gptype);
		void bindGpuProgramParameters_internal(GpuProgramType gptype, 
			GpuProgramParametersSharedPtr params, UINT16 variabilityMask);

		// Low-level overridden members, mainly for internal use
		void setPointParameters_internal(float size, bool attenuationEnabled, 
			float constant, float linear, float quadratic, float minSize, float maxSize);
		void setTexture_internal(size_t unit, bool enabled, const TexturePtr &texPtr);
		void setVertexTexture_internal(size_t unit, const TexturePtr& tex);
		void disableTextureUnit_internal(size_t texUnit);
        void setTextureAddressingMode_internal(size_t stage, const SamplerState::UVWAddressingMode& uvw);
        void setTextureBorderColor_internal(size_t stage, const Color& colour);
		void setTextureMipmapBias_internal(size_t unit, float bias);
		void setSceneBlending_internal( SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendOperation op );
		void setSeparateSceneBlending_internal( SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendFactor sourceFactorAlpha, SceneBlendFactor destFactorAlpha, SceneBlendOperation op, SceneBlendOperation alphaOp );
		void setAlphaRejectSettings_internal( CompareFunction func, unsigned char value, bool alphaToCoverage );
		void setViewport_internal(const Viewport& vp);		
		void beginFrame_internal();
		void endFrame_internal();		
		void setCullingMode_internal( CullingMode mode );
		void setDepthBufferParams_internal( bool depthTest = true, bool depthWrite = true, CompareFunction depthFunction = CMPF_LESS_EQUAL );
		void setDepthBufferCheckEnabled_internal( bool enabled = true );
		void setColorBufferWriteEnabled_internal(bool red, bool green, bool blue, bool alpha);
		void setDepthBufferWriteEnabled_internal(bool enabled = true);
		void setDepthBufferFunction_internal( CompareFunction func = CMPF_LESS_EQUAL );
		void setDepthBias_internal(float constantBias, float slopeScaleBias);
		void convertProjectionMatrix(const Matrix4& matrix, Matrix4& dest, bool forGpuProgram = false);
		void setPolygonMode_internal(PolygonMode level);
        void setTextureFiltering_internal(size_t unit, FilterType ftype, FilterOptions filter);
		void setTextureAnisotropy_internal(size_t unit, unsigned int maxAnisotropy);
		void setVertexDeclaration_internal(VertexDeclarationPtr decl);
		void setVertexBufferBinding_internal(VertexBufferBinding* binding);
        void render_internal(const RenderOperation& op);

        void setScissorTest_internal(bool enabled, size_t left = 0, size_t top = 0, size_t right = 800, size_t bottom = 600);
        void clearFrameBuffer_internal(unsigned int buffers, 
            const Color& colour = Color::Black, 
            float depth = 1.0f, unsigned short stencil = 0);

        float getHorizontalTexelOffset();
        float getVerticalTexelOffset();
        float getMinimumDepthInputValue();
        float getMaximumDepthInputValue();
		VertexElementType getColorVertexElementType() const;

		static D3D9ResourceManager* getResourceManager();
		static D3D9DeviceManager* getDeviceManager();
		static IDirect3D9* getDirect3D9();
		static UINT	getResourceCreationDeviceCount();
		static IDirect3DDevice9* getResourceCreationDevice(UINT index);
		static IDirect3DDevice9* getActiveD3D9Device();

	protected:	
		/// Notify when a device has been lost.
		void notifyOnDeviceLost(D3D9Device* device);

		/// Notify when a device has been reset.
		void notifyOnDeviceReset(D3D9Device* device);
		
		typedef map<RenderTarget*, ZBufferRef>::type TargetDepthStencilMap;
		TargetDepthStencilMap mCheckedOutTextures;

	private:
		friend class D3D9Device;
		friend class D3D9DeviceManager;		
	};
}
#endif
