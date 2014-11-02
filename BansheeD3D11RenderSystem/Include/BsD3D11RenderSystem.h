#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsRenderSystem.h"

namespace BansheeEngine
{
	/**
	 * @brief	Implementation of a render system using DirectX 11. Provides abstracted
	 *			access to various low level DX11 methods.
	 */
	class BS_D3D11_EXPORT D3D11RenderSystem : public RenderSystem
	{
	public:
		D3D11RenderSystem();
		~D3D11RenderSystem();

		/**
		 * @copydoc RenderSystem::getName
		 */
		const String& getName() const;
		
		/**
		 * @copydoc RenderSystem::getShadingLanguageName
		 */
		const String& getShadingLanguageName() const;

		/**
		 * @copydoc	RenderSystem::setBlendState
		 */
		void setBlendState(const BlendStatePtr& blendState);

		/**
		 * @copydoc	RenderSystem::setRasterizerState
		 */
		void setRasterizerState(const RasterizerStatePtr& rasterizerState);

		/**
		 * @copydoc	RenderSystem::setDepthStencilState
		 */
		void setDepthStencilState(const DepthStencilStatePtr& depthStencilState, UINT32 stencilRefValue);

		/**
		 * @copydoc	RenderSystem::setSamplerState
		 */
		void setSamplerState(GpuProgramType gptype, UINT16 texUnit, const SamplerStatePtr& samplerState);

		/**
		 * @copydoc	RenderSystem::setTexture
		 */
		void setTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const TexturePtr &texPtr);

		/**
		 * @copydoc	RenderSystem::setLoadStoreTexture
		 */
		void setLoadStoreTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const TexturePtr& texPtr,
			const TextureSurface& surface);

		/**
		 * @copydoc	RenderSystem::disableTextureUnit
		 */
		void disableTextureUnit(GpuProgramType gptype, UINT16 texUnit);

		/**
		 * @copydoc	RenderSystem::beginFrame
		 */
		void beginFrame();

		/**
		 * @copydoc	RenderSystem::endFrame
		 */
		void endFrame();

		/**
		 * @copydoc RenderSystem::clearRenderTarget
		 */
		void clearRenderTarget(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0);

		/**
		 * @copydoc RenderSystem::clearViewport
		 */
		void clearViewport(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0);

		/**
		 * @copydoc	RenderSystem::setRenderTarget
		 */
		void setRenderTarget(RenderTargetPtr target);

		/**
		 * @copydoc	RenderSystem::setViewport
		 */
		void setViewport(Viewport vp);

		/**
		 * @copydoc	RenderSystem::setScissorRect
		 */
		void setScissorRect(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom);

		/**
		 * @copydoc	RenderSystem::setVertexBuffers
		 */
		void setVertexBuffers(UINT32 index, VertexBufferPtr* buffers, UINT32 numBuffers);

		/**
		 * @copydoc	RenderSystem::setIndexBuffer
		 */
		void setIndexBuffer(const IndexBufferPtr& buffer);

		/**
		 * @copydoc	RenderSystem::setVertexDeclaration
		 */
		void setVertexDeclaration(VertexDeclarationPtr vertexDeclaration);

		/**
		 * @copydoc	RenderSystem::setDrawOperation
		 */
		void setDrawOperation(DrawOperationType op);

		/**
		 * @copydoc	RenderSystem::draw
		 */
		void draw(UINT32 vertexOffset, UINT32 vertexCount);

		/**
		 * @copydoc	RenderSystem::drawIndexed
		 */
		void drawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount);

		/** 
		 * @copydoc RenderSystem::bindGpuProgram
		 */
		void bindGpuProgram(HGpuProgram prg);

		/** 
		 * @copydoc RenderSystem::unbindGpuProgram
		 */
		void unbindGpuProgram(GpuProgramType gptype);

		/** 
		 * @copydoc RenderSystem::bindGpuParams
		 */
		void bindGpuParams(GpuProgramType gptype, GpuParamsPtr params);
		
		/**
		 * @copydoc	RenderSystem::setClipPlanesImpl
		 */
		void setClipPlanesImpl(const PlaneList& clipPlanes);

		/**
		 * @copydoc	RenderSystem::convertProjectionMatrix
		 */
		void convertProjectionMatrix(const Matrix4& matrix, Matrix4& dest);

		/**
		 * @copydoc	RenderSystem::getColorVertexElementType
		 */
		VertexElementType getColorVertexElementType() const;

		/**
		 * @copydoc	RenderSystem::getHorizontalTexelOffset
		 */
		float getHorizontalTexelOffset();

		/**
		 * @copydoc	RenderSystem::getVerticalTexelOffset
		 */
		float getVerticalTexelOffset();

		/**
		 * @copydoc	RenderSystem::getMinimumDepthInputValue
		 */
		float getMinimumDepthInputValue();

		/**
		 * @copydoc	RenderSystem::getMaximumDepthInputValue
		 */
		float getMaximumDepthInputValue();

		/************************************************************************/
		/* 				Internal use by DX11 RenderSystem only                  */
		/************************************************************************/

		/**
		 * @brief	Determines DXGI multisample settings from the provided parameters.
		 *
		 * @param	multisampleCount	Number of requested samples.
		 * @param	format				Pixel format used by the render target.
		 * @param	outputSampleDesc	Output structure that will contain the requested multisample settings.
		 */
		void determineMultisampleSettings(UINT32 multisampleCount, DXGI_FORMAT format, DXGI_SAMPLE_DESC* outputSampleDesc);

		/**
		 * @brief	Returns the main DXGI factory object.
		 */
		IDXGIFactory* getDXGIFactory() const { return mDXGIFactory; }

		/**
		 * @brief	Returns the primary DX11 device object.
		 */
		D3D11Device& getPrimaryDevice() const { return *mDevice; }
		
		/**
		 * @brief	Returns information describing all available drivers.
		 */
		D3D11DriverList* getDriverList() const { return mDriverList; }

	protected:
		friend class D3D11RenderSystemFactory;

		/**
		 * @copydoc	RenderSystem::initialize_internal
		 */
		void initialize_internal(AsyncOp& asyncOp);

		/**
		 * @copydoc	RenderSystem::destroy_internal
		 */
        void destroy_internal();

		/**
		 * @brief	Creates or retrieves a proper input layout depending on the currently set vertex shader
		 * 			and vertex buffer.
		 *
		 *			Applies the input layout to the pipeline.
		 */
		void applyInputLayout();

		/**
		 * @brief	Creates and populates a set of render system capabilities describing which functionality
		 *			is available.
		 */
		RenderSystemCapabilities* createRenderSystemCapabilities() const;

	private:
		IDXGIFactory* mDXGIFactory;
		D3D11Device* mDevice;

		D3D11DriverList* mDriverList;
		D3D11Driver* mActiveD3DDriver;

		D3D_FEATURE_LEVEL mFeatureLevel;

		D3D11HLSLProgramFactory* mHLSLFactory;
		D3D11InputLayoutManager* mIAManager;

		std::pair<TexturePtr, TextureViewPtr> mBoundUAVs[D3D11_PS_CS_UAV_REGISTER_COUNT];

		UINT32 mStencilRef;
		D3D11_VIEWPORT mViewport;
		D3D11_RECT mScissorRect;

		VertexDeclarationPtr mActiveVertexDeclaration;
		D3D11GpuProgramPtr mActiveVertexShader;

		DrawOperationType mActiveDrawOp;
	};
}