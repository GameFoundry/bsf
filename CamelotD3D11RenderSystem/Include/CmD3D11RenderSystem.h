#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmRenderSystem.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11RenderSystem : public RenderSystem
	{
	public:
		static D3D11Device& getPrimaryDevice();

		void determineFSAASettings(UINT32 fsaa, const String& fsaaHint, DXGI_FORMAT format, DXGI_SAMPLE_DESC* outFSAASettings);
		bool checkTextureFilteringSupported(TextureType ttype, PixelFormat format, int usage);

		virtual const String& getName() const;

		virtual void setSamplerState(UINT16 texUnit, const SamplerState& samplerState);
		virtual void setBlendState(const BlendState& blendState);
		virtual void setRasterizerState(const RasterizerState& rasterizerState);
		virtual void setDepthStencilState(const DepthStencilState& depthStencilState);

		virtual void setTexture(UINT16 unit, bool enabled, const TexturePtr &texPtr );

		virtual void setStencilRefValue(UINT32 refValue);

		virtual void beginFrame();
		virtual void endFrame();
		virtual void clearFrameBuffer(unsigned int buffers, const Color& color = Color::Black, float depth = 1.0f, unsigned short stencil = 0);

		virtual void setRenderTarget(RenderTarget* target);
		virtual void setViewport(const Viewport& vp);
		virtual void setScissorRect(UINT32 left = 0, UINT32 top = 0, UINT32 right = 800, UINT32 bottom = 600);

		virtual void setVertexDeclaration(VertexDeclarationPtr decl);
		virtual void setVertexBufferBinding(VertexBufferBinding* binding);

		virtual void bindGpuProgramParameters(GpuProgramType gptype, GpuProgramParametersSharedPtr params, UINT16 variabilityMask);

		
		virtual void setClipPlanesImpl(const PlaneList& clipPlanes);

		virtual RenderSystemCapabilities* createRenderSystemCapabilities() const;
		virtual void initialiseFromRenderSystemCapabilities(RenderSystemCapabilities* caps);

		virtual String getErrorDescription(long errorNumber) const;

		virtual void convertProjectionMatrix(const Matrix4& matrix, Matrix4& dest, bool forGpuProgram = false);
		virtual VertexElementType getColorVertexElementType() const;
		virtual float getHorizontalTexelOffset();
		virtual float getVerticalTexelOffset();
		virtual float getMinimumDepthInputValue();
		virtual float getMaximumDepthInputValue();

	protected:

	private:
	};
}