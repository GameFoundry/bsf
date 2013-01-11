#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmRenderSystem.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11RenderSystem : public RenderSystem
	{
	public:
		~D3D11RenderSystem();

		/**
		 * @copydoc RenderSystem::getName()
		 */
		const String& getName() const;
		
		/**
		 * @copydoc RenderSystem::getShadingLanguageName()
		 */
		const String& getShadingLanguageName() const;

		void setBlendState(const BlendStatePtr& blendState);
		void setRasterizerState(const RasterizerStatePtr& rasterizerState);
		void setDepthStencilState(const DepthStencilStatePtr& depthStencilState, UINT32 stencilRefValue);

		void setSamplerState(GpuProgramType gptype, UINT16 texUnit, const SamplerStatePtr& samplerState);
		void setTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const TexturePtr &texPtr);
		void disableTextureUnit(GpuProgramType gptype, UINT16 texUnit);

		void beginFrame();
		void endFrame();
		void clear(RenderTargetPtr target, unsigned int buffers, const Color& color = Color::Black, float depth = 1.0f, unsigned short stencil = 0);

		void setRenderTarget(RenderTarget* target);
		void setViewport(const Viewport& vp);
		void setScissorRect(UINT32 left = 0, UINT32 top = 0, UINT32 right = 800, UINT32 bottom = 600);

		void setVertexBuffer(UINT32 index, const VertexBufferPtr& buffer);
		void setIndexBuffer(const IndexBufferPtr& buffer);
		void setVertexDeclaration(VertexDeclarationPtr vertexDeclaration);
		void setDrawOperation(DrawOperationType op);

		/** @copydoc RenderSystem::draw() */
		void draw(UINT32 vertexCount);

		/** @copydoc RenderSystem::drawIndexed() */
		void drawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexCount);

		/** @copydoc RenderSystem::bindGpuProgram() */
		void bindGpuProgram(GpuProgramHandle prg);
		/** @copydoc RenderSystem::unbindGpuProgram() */
		void unbindGpuProgram(GpuProgramType gptype);
		/** @copydoc RenderSystem::bindGpuParams() */
		void bindGpuParams(GpuProgramType gptype, GpuParamsPtr params);
		
		void setClipPlanesImpl(const PlaneList& clipPlanes);

		RenderSystemCapabilities* createRenderSystemCapabilities() const;
		void initialiseFromRenderSystemCapabilities(RenderSystemCapabilities* caps);

		String getErrorDescription(long errorNumber) const;

		void convertProjectionMatrix(const Matrix4& matrix, Matrix4& dest, bool forGpuProgram = false);
		VertexElementType getColorVertexElementType() const;
		float getHorizontalTexelOffset();
		float getVerticalTexelOffset();
		float getMinimumDepthInputValue();
		float getMaximumDepthInputValue();

		/************************************************************************/
		/* 				Internal use by DX11 RenderSystem only                  */
		/************************************************************************/

		void determineFSAASettings(UINT32 fsaa, const String& fsaaHint, DXGI_FORMAT format, DXGI_SAMPLE_DESC* outFSAASettings);
		bool checkTextureFilteringSupported(TextureType ttype, PixelFormat format, int usage);

		IDXGIFactory* getDXGIFactory() const { return mDXGIFactory; }
		D3D11Device& getPrimaryDevice() const { return *mDevice; }
		
	protected:
		friend class D3D11RenderSystemFactory;
		D3D11RenderSystem();

		/**
		 * @copydoc	RenderSystem::initialize_internal().
		 */
		void initialize_internal();

		/**
		 * @copydoc	RenderSystem::destroy_internal().
		 */
        void destroy_internal();

	private:
		IDXGIFactory*		mDXGIFactory;
		D3D11Device*		mDevice;

		D3D11DriverList*	mDriverList;
		D3D11Driver*		mActiveD3DDriver;

		D3D_FEATURE_LEVEL	mFeatureLevel;

		D3D11HLSLProgramFactory* mHLSLFactory;

		// State variables
		UINT32				mStencilRef;
		D3D11_VIEWPORT		mViewport;
		D3D11_RECT			mScissorRect;
	};
}