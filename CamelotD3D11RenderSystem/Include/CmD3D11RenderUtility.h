#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmModule.h"

namespace CamelotFramework
{
	class D3D11RenderUtility : public Module<D3D11RenderUtility>
	{
	public:
		D3D11RenderUtility(D3D11Device* device);
		~D3D11RenderUtility();

		void drawClearQuad(float clipLeft, float clipWidth, float clipTop, float clipHeight, UINT32 clearBuffers, const Color& color, float depth, UINT16 stencil);

	protected:
		D3D11Device* mDevice;

		void initClearQuadResources();

		ID3D11Buffer* mClearQuadIB;
		ID3D11Buffer* mClearQuadVB;
		ID3D11InputLayout* mClearQuadIL;
		ID3D11VertexShader* mClearQuadVS;
		ID3D11PixelShader* mClearQuadPS;

		BlendStatePtr mClearQuadBlendState;
		RasterizerStatePtr mClearQuadRasterizerState;
		DepthStencilStatePtr mClearQuadDSState_NoD_NoS;
		DepthStencilStatePtr mClearQuadDSState_YesD_NoS;
		DepthStencilStatePtr mClearQuadDSState_YesD_YesS;
		DepthStencilStatePtr mClearQuadDSState_NoD_YesS;
	};
}