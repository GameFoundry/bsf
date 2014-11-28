#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsRenderAPI.h"

namespace BansheeEngine
{
	/**
	 * @brief	Implementation of a render system using DirectX 11. Provides abstracted
	 *			access to various low level DX11 methods.
	 */
	class BS_D3D11_EXPORT D3D11RenderAPI : public RenderAPICore
	{
	public:
		D3D11RenderAPI();
		~D3D11RenderAPI();

		/**
		 * @copydoc RenderAPICore::getName
		 */
		const String& getName() const;
		
		/**
		 * @copydoc RenderAPICore::getShadingLanguageName
		 */
		const String& getShadingLanguageName() const;

		/**
		 * @copydoc	RenderAPICore::setBlendState
		 */
		void setBlendState(const SPtr<BlendStateCore>& blendState);

		/**
		 * @copydoc	RenderAPICore::setRasterizerState
		 */
		void setRasterizerState(const SPtr<RasterizerStateCore>& rasterizerState);

		/**
		 * @copydoc	RenderAPICore::setDepthStencilState
		 */
		void setDepthStencilState(const SPtr<DepthStencilStateCore>& depthStencilState, UINT32 stencilRefValue);

		/**
		 * @copydoc	RenderAPICore::setSamplerState
		 */
		void setSamplerState(GpuProgramType gptype, UINT16 texUnit, const SPtr<SamplerStateCore>& samplerState);

		/**
		 * @copydoc	RenderAPICore::setTexture
		 */
		void setTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const SPtr<TextureCore>& texPtr);

		/**
		 * @copydoc	RenderAPICore::setLoadStoreTexture
		 */
		void setLoadStoreTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const SPtr<TextureCore>& texPtr,
			const TextureSurface& surface);

		/**
		 * @copydoc	RenderAPICore::disableTextureUnit
		 */
		void disableTextureUnit(GpuProgramType gptype, UINT16 texUnit);

		/**
		 * @copydoc	RenderAPICore::beginFrame
		 */
		void beginFrame();

		/**
		 * @copydoc	RenderAPICore::endFrame
		 */
		void endFrame();

		/**
		 * @copydoc RenderAPICore::clearRenderTarget
		 */
		void clearRenderTarget(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0);

		/**
		 * @copydoc RenderAPICore::clearViewport
		 */
		void clearViewport(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0);

		/**
		 * @copydoc	RenderAPICore::setRenderTarget
		 */
		void setRenderTarget(const SPtr<RenderTargetCore>& target);

		/**
		 * @copydoc	RenderAPICore::setViewport
		 */
		void setViewport(const Rect2& vp);

		/**
		 * @copydoc	RenderAPICore::setScissorRect
		 */
		void setScissorRect(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom);

		/**
		 * @copydoc	RenderAPICore::setVertexBuffers
		 */
		void setVertexBuffers(UINT32 index, SPtr<VertexBufferCore>* buffers, UINT32 numBuffers);

		/**
		 * @copydoc	RenderAPICore::setIndexBuffer
		 */
		void setIndexBuffer(const SPtr<IndexBufferCore>& buffer);

		/**
		 * @copydoc	RenderAPICore::setVertexDeclaration
		 */
		void setVertexDeclaration(const SPtr<VertexDeclarationCore>& vertexDeclaration);

		/**
		 * @copydoc	RenderAPICore::setDrawOperation
		 */
		void setDrawOperation(DrawOperationType op);

		/**
		 * @copydoc	RenderAPICore::draw
		 */
		void draw(UINT32 vertexOffset, UINT32 vertexCount);

		/**
		 * @copydoc	RenderAPICore::drawIndexed
		 */
		void drawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount);

		/** 
		 * @copydoc RenderAPICore::bindGpuProgram
		 */
		void bindGpuProgram(const SPtr<GpuProgramCore>& prg);

		/** 
		 * @copydoc RenderAPICore::unbindGpuProgram
		 */
		void unbindGpuProgram(GpuProgramType gptype);

		/** 
		 * @copydoc RenderAPICore::bindGpuParams
		 */
		void bindGpuParams(GpuProgramType gptype, const SPtr<GpuParamsCore>& params);
		
		/**
		 * @copydoc	RenderAPICore::setClipPlanesImpl
		 */
		void setClipPlanesImpl(const PlaneList& clipPlanes);

		/**
		 * @copydoc	RenderAPICore::convertProjectionMatrix
		 */
		void convertProjectionMatrix(const Matrix4& matrix, Matrix4& dest);

		/**
		 * @copydoc	RenderAPICore::getColorVertexElementType
		 */
		VertexElementType getColorVertexElementType() const;

		/**
		 * @copydoc	RenderAPICore::getHorizontalTexelOffset
		 */
		float getHorizontalTexelOffset();

		/**
		 * @copydoc	RenderAPICore::getVerticalTexelOffset
		 */
		float getVerticalTexelOffset();

		/**
		 * @copydoc	RenderAPICore::getMinimumDepthInputValue
		 */
		float getMinimumDepthInputValue();

		/**
		 * @copydoc	RenderAPICore::getMaximumDepthInputValue
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
		friend class D3D11RenderAPIFactory;

		/**
		 * @copydoc	RenderAPICore::initializePrepare
		 */
		void initializePrepare();

		/**
		 * @copydoc	RenderAPICore::initializeFinalize
		 */
		void initializeFinalize(const SPtr<RenderWindowCore>& primaryWindow);

		/**
		 * @copydoc	RenderAPICore::destroy_internal
		 */
        void destroyCore();

		/**
		 * @brief	Creates or retrieves a proper input layout depending on the currently set vertex shader
		 * 			and vertex buffer.
		 *
		 *			Applies the input layout to the pipeline.
		 */
		void applyInputLayout();

		/**
		 * @brief	Recalculates actual viewport dimensions based on currently 
		 *			set viewport normalized dimensions and render target and applies
		 *			them for further rendering.
		 */
		void applyViewport();

		/**
		 * @brief	Creates and populates a set of render system capabilities describing which functionality
		 *			is available.
		 */
		RenderAPICapabilities* createRenderSystemCapabilities() const;

	private:
		IDXGIFactory* mDXGIFactory;
		D3D11Device* mDevice;

		D3D11DriverList* mDriverList;
		D3D11Driver* mActiveD3DDriver;

		D3D_FEATURE_LEVEL mFeatureLevel;

		D3D11HLSLProgramFactory* mHLSLFactory;
		D3D11InputLayoutManager* mIAManager;

		std::pair<SPtr<TextureCore>, TextureViewPtr> mBoundUAVs[D3D11_PS_CS_UAV_REGISTER_COUNT];

		UINT32 mStencilRef;
		Rect2 mViewportNorm;
		D3D11_VIEWPORT mViewport;
		D3D11_RECT mScissorRect;

		SPtr<VertexDeclarationCore> mActiveVertexDeclaration;
		SPtr<D3D11GpuProgramCore> mActiveVertexShader;

		DrawOperationType mActiveDrawOp;
	};
}