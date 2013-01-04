#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmRenderSystem.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11RenderSystem : public RenderSystem
	{
	public:
		~D3D11RenderSystem();

		const String& getName() const;
		
		void setBlendState(const BlendState& blendState);
		void setRasterizerState(const RasterizerState& rasterizerState);
		void setDepthStencilState(const DepthStencilState& depthStencilState);

		void setSamplerState(GpuProgramType gptype, UINT16 texUnit, const SamplerState& samplerState);
		void setTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const TexturePtr &texPtr);
		void disableTextureUnit(GpuProgramType gptype, UINT16 texUnit);

		void setStencilRefValue(UINT32 refValue);

		void beginFrame();
		void endFrame();
		void clearFrameBuffer(unsigned int buffers, const Color& color = Color::Black, float depth = 1.0f, unsigned short stencil = 0);

		void setRenderTarget(RenderTarget* target);
		void setViewport(const Viewport& vp);
		void setScissorRect(UINT32 left = 0, UINT32 top = 0, UINT32 right = 800, UINT32 bottom = 600);

		void setVertexDeclaration(VertexDeclarationPtr decl);
		void setVertexBufferBinding(VertexBufferBinding* binding);

		void bindGpuProgram(GpuProgramHandle prg);
		void unbindGpuProgram(GpuProgramType gptype);
		void bindGpuProgramParameters(GpuProgramType gptype, GpuProgramParametersSharedPtr params, UINT16 variabilityMask);
		
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
	};
}