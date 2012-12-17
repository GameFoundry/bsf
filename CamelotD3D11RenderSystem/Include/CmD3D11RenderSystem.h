#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmRenderSystem.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11RenderSystem : public RenderSystem
	{
	public:
		//D3D11RenderSystem();
		//~D3D11RenderSystem();

		//const String& getName() const;
		//void startUp_internal();
		//void shutdown();

		//void setStencilCheckEnabled_internal(bool enabled);
		//void setStencilBufferParams_internal(CompareFunction func = CMPF_ALWAYS_PASS, 
		//	UINT32 refValue = 0, UINT32 mask = 0xFFFFFFFF, 
		//	StencilOperation stencilFailOp = SOP_KEEP, 
		//	StencilOperation depthFailOp = SOP_KEEP,
		//	StencilOperation passOp = SOP_KEEP, 
		//	bool twoSidedOperation = false);

		//void createRenderWindow_internal(const String &name, unsigned int width, unsigned int height, bool fullScreen, const NameValuePairList& miscParams, AsyncOp& asyncOp);
		//void destroyRenderTarget_internal(RenderTarget* renderTarget);
		//void setRenderTarget_internal(RenderTarget *target);

		//void bindGpuProgram_internal(GpuProgramHandle prg);
		//void unbindGpuProgram_internal(GpuProgramType gptype);
		//void bindGpuProgramParameters_internal(GpuProgramType gptype, GpuProgramParametersSharedPtr params, UINT16 variabilityMask);

		//void setPointParameters_internal(float size, bool attenuationEnabled, float constant, float linear, float quadratic, float minSize, float maxSize);
		//void setTexture_internal(UINT16 unit, bool enabled, const TexturePtr &texPtr);
		//void setVertexTexture_internal(UINT16 unit, const TexturePtr& tex);
		//void disableTextureUnit_internal(UINT16 texUnit);
		//void setTextureAddressingMode_internal(UINT16 stage, const SamplerState::UVWAddressingMode& uvw);
		//void setTextureBorderColor_internal(UINT16 stage, const Color& colour);
		//void setTextureMipmapBias_internal(UINT16 unit, float bias);
		//void setSceneBlending_internal( SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendOperation op );
		//void setSeparateSceneBlending_internal( SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendFactor sourceFactorAlpha, SceneBlendFactor destFactorAlpha, SceneBlendOperation op, SceneBlendOperation alphaOp );
		//void setAlphaRejectSettings_internal( CompareFunction func, unsigned char value, bool alphaToCoverage );
		//void setViewport_internal(const Viewport& vp);				
		//void setCullingMode_internal(CullingMode mode);
		//void setDepthBufferParams_internal( bool depthTest = true, bool depthWrite = true, CompareFunction depthFunction = CMPF_LESS_EQUAL );
		//void setDepthBufferCheckEnabled_internal( bool enabled = true );
		//void setColorBufferWriteEnabled_internal(bool red, bool green, bool blue, bool alpha);
		//void setDepthBufferWriteEnabled_internal(bool enabled = true);
		//void setDepthBufferFunction_internal( CompareFunction func = CMPF_LESS_EQUAL );
		//void setDepthBias_internal(float constantBias, float slopeScaleBias);
		//void setPolygonMode_internal(PolygonMode level);
		//void setTextureFiltering_internal(UINT16 unit, FilterType ftype, FilterOptions filter);
		//void setTextureAnisotropy_internal(UINT16 unit, unsigned int maxAnisotropy);
		//void setVertexDeclaration_internal(VertexDeclarationPtr decl);
		//void setVertexBufferBinding_internal(VertexBufferBinding* binding);

		//void render_internal(const RenderOperation& op);
		//void beginFrame_internal();
		//void endFrame_internal();

		//void setScissorTest_internal(bool enabled, UINT32 left = 0, UINT32 top = 0, UINT32 right = 800, UINT32 bottom = 600);
		//void clearFrameBuffer_internal(unsigned int buffers, const Color& colour = Color::Black, float depth = 1.0f, unsigned short stencil = 0);

		//float getHorizontalTexelOffset();
		//float getVerticalTexelOffset();
		//float getMinimumDepthInputValue();
		//float getMaximumDepthInputValue();
		//VertexElementType getColorVertexElementType() const;
		//void convertProjectionMatrix(const Matrix4& matrix, Matrix4& dest, bool forGpuProgram = false);

		static D3D11Device& getPrimaryDevice();
	protected:

	private:
	};
}