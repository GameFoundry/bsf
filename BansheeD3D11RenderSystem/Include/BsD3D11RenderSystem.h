#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsRenderSystem.h"

namespace BansheeEngine
{
	class BS_D3D11_EXPORT D3D11RenderSystem : public RenderSystem
	{
	public:
		D3D11RenderSystem();
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

		/**
		 * @copydoc RenderSystem::clearRenderTarget()
		 */
		void clearRenderTarget(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0);

		/**
		 * @copydoc RenderSystem::clearViewport()
		 */
		void clearViewport(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0);

		void setRenderTarget(RenderTargetPtr target);
		void setViewport(Viewport vp);
		void setScissorRect(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom);

		void setVertexBuffers(UINT32 index, VertexBufferPtr* buffers, UINT32 numBuffers);
		void setIndexBuffer(const IndexBufferPtr& buffer);
		void setVertexDeclaration(VertexDeclarationPtr vertexDeclaration);
		void setDrawOperation(DrawOperationType op);

		/** @copydoc RenderSystem::draw() */
		void draw(UINT32 vertexOffset, UINT32 vertexCount);

		/** @copydoc RenderSystem::drawIndexed() */
		void drawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount);

		/** @copydoc RenderSystem::bindGpuProgram() */
		void bindGpuProgram(HGpuProgram prg);
		/** @copydoc RenderSystem::unbindGpuProgram() */
		void unbindGpuProgram(GpuProgramType gptype);
		/** @copydoc RenderSystem::bindGpuParams() */
		void bindGpuParams(GpuProgramType gptype, BindableGpuParams& params);
		
		void setClipPlanesImpl(const PlaneList& clipPlanes);

		RenderSystemCapabilities* createRenderSystemCapabilities() const;
		void initialiseFromRenderSystemCapabilities(RenderSystemCapabilities* caps);

		void convertProjectionMatrix(const Matrix4& matrix, Matrix4& dest, bool forGpuProgram = false);
		VertexElementType getColorVertexElementType() const;
		float getHorizontalTexelOffset();
		float getVerticalTexelOffset();
		float getMinimumDepthInputValue();
		float getMaximumDepthInputValue();

		/************************************************************************/
		/* 				Internal use by DX11 RenderSystem only                  */
		/************************************************************************/

		void determineMultisampleSettings(UINT32 multisampleCount, const String& multisampleHint, DXGI_FORMAT format, DXGI_SAMPLE_DESC* outputSampleDesc);
		bool checkTextureFilteringSupported(TextureType ttype, PixelFormat format, int usage);

		IDXGIFactory* getDXGIFactory() const { return mDXGIFactory; }
		D3D11Device& getPrimaryDevice() const { return *mDevice; }
		
		/**
		 * @brief	Returns a total number of outputs (e.g. monitors), across all adapters.
		 */
		D3D11DriverList* getDriverList() const { return mDriverList; }

	protected:
		friend class D3D11RenderSystemFactory;

		/**
		 * @copydoc	RenderSystem::initialize_internal().
		 */
		void initialize_internal(AsyncOp& asyncOp);

		/**
		 * @copydoc	RenderSystem::destroy_internal().
		 */
        void destroy_internal();

		/**
		 * @brief	Creates or retrieves a proper input layout depending on the currently set vertex shader
		 * 			and vertex buffer.
		 *
		 *			Applies the input layout to the pipeline.
		 */
		void applyInputLayout();

	private:
		IDXGIFactory* mDXGIFactory;
		D3D11Device* mDevice;

		D3D11DriverList* mDriverList;
		D3D11Driver* mActiveD3DDriver;

		D3D_FEATURE_LEVEL mFeatureLevel;

		D3D11HLSLProgramFactory* mHLSLFactory;
		D3D11InputLayoutManager* mIAManager;

		UINT32 mStencilRef;
		D3D11_VIEWPORT mViewport;
		D3D11_RECT mScissorRect;

		VertexDeclarationPtr mActiveVertexDeclaration;
		D3D11GpuProgramPtr mActiveVertexShader;

		DrawOperationType mActiveDrawOp;
	};
}