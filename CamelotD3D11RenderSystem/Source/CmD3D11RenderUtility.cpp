#include "CmD3D11RenderUtility.h"
#include "CmD3D11Device.h"
#include "CmVector3.h"
#include "CmColor.h"
#include "CmRect.h"

namespace CamelotFramework
{
	struct ClearVertex
	{
		Vector3 pos;
		UINT32 col;
	};

	D3D11RenderUtility::D3D11RenderUtility(D3D11Device* device)
		:mDevice(device), mClearQuadIB(nullptr), mClearQuadVB(nullptr), 
		mClearQuadIL(nullptr), mClearQuadVS(nullptr), mClearQuadPS(nullptr)
	{

	}

	D3D11RenderUtility::~D3D11RenderUtility()
	{
		SAFE_RELEASE(mClearQuadPS);
		SAFE_RELEASE(mClearQuadVS);
		SAFE_RELEASE(mClearQuadIL);
		SAFE_RELEASE(mClearQuadIB);
		SAFE_RELEASE(mClearQuadVB);

		// TODO - Clear up all resources
	}

	void D3D11RenderUtility::drawClearQuad(float clipLeft, float clipWidth, float clipTop, float clipHeight, 
		UINT32 clearBuffers, const Color& color, float depth, UINT16 stencil)
	{
		// TODO - Set up states
		// TODO - Make sure to apply proper states depending on "clearBuffers" flags. Don't forget about stencil

		// TODO - How smart it is to update buffer right before drawing it!? (cache the clip area)
		ClearVertex vertexData[4];
		vertexData[0].pos = Vector3(clipLeft, clipTop, depth);
		vertexData[1].pos = Vector3(clipLeft + clipWidth, clipTop, depth);
		vertexData[2].pos = Vector3(clipLeft, clipTop + clipHeight, depth);
		vertexData[3].pos = Vector3(clipLeft + clipWidth, clipTop + clipHeight, depth);

		vertexData[0].col = color.getAsBGRA();
		vertexData[1].col = color.getAsBGRA();
		vertexData[2].col = color.getAsBGRA();
		vertexData[3].col = color.getAsBGRA();

		mDevice->getImmediateContext()->UpdateSubresource(mClearQuadVB, 0, nullptr, vertexData, 0, sizeof(vertexData));

		mDevice->getImmediateContext()->VSSetShader(mClearQuadVS, nullptr, 0);
		mDevice->getImmediateContext()->PSSetShader(mClearQuadPS, nullptr, 0);

		ID3D11Buffer* buffers[1];
		buffers[0] = mClearQuadVB;

		UINT32 strides[1] = { sizeof(ClearVertex) };
		UINT32 offsets[1] = { 0 };

		mDevice->getImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mDevice->getImmediateContext()->IASetIndexBuffer(mClearQuadIB, DXGI_FORMAT_R16_UINT, 0);
		mDevice->getImmediateContext()->IASetVertexBuffers(0, 1, buffers, strides, offsets);
		mDevice->getImmediateContext()->IASetInputLayout(mClearQuadIL);

		mDevice->getImmediateContext()->DrawIndexed(4, 0, 0);

	}

	void D3D11RenderUtility::initClearQuadResources()
	{
		String vsShaderCode = "										\
						void main(									\
						in float3 inPos : POSITION,					\
						in float4 color : COLOR0,					\
						out float4 oPosition : SV_Position,			\
						out float4 oColor : COLOR0)					\
						{											\
							oPosition = float4(inPos.xyz, 1);		\
							oColor = color;							\
						}											\
						";

		String psShaderCode = "float4 ps_main(in float4 inPos : SV_Position, float4 color : COLOR) : SV_Target	\
								{ return color; }";

		HRESULT hr;
		ID3DBlob* microcode = nullptr;
		ID3DBlob* errors = nullptr;

		// Compile pixel shader
		hr = D3DCompile(psShaderCode.c_str(), psShaderCode.size(), nullptr, nullptr, nullptr, "main", "ps_4_0",
			D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR, 0, &microcode, &errors);

		if (FAILED(hr))
		{
			String message = "Cannot assemble D3D11 high-level shader. Errors:\n" +
				String(static_cast<const char*>(errors->GetBufferPointer()));

			SAFE_RELEASE(errors);
			CM_EXCEPT(RenderingAPIException, message);
		}

		SAFE_RELEASE(errors);

		hr = mDevice->getD3D11Device()->CreatePixelShader(static_cast<DWORD*>(microcode->GetBufferPointer()), 
			microcode->GetBufferSize(), mDevice->getClassLinkage(), &mClearQuadPS);

		if (FAILED(hr))
		{
			SAFE_RELEASE(microcode);
			CM_EXCEPT(RenderingAPIException, "Cannot create D3D11 high-level shader.");
		}

		// Compile vertex shader
		hr = D3DCompile(vsShaderCode.c_str(), vsShaderCode.size(), nullptr, nullptr, nullptr, "main", "vs_4_0",
			D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR, 0, &microcode, &errors);

		if (FAILED(hr))
		{
			String message = "Cannot assemble D3D11 high-level shader. Errors:\n" +
				String(static_cast<const char*>(errors->GetBufferPointer()));

			SAFE_RELEASE(errors);
			CM_EXCEPT(RenderingAPIException, message);
		}

		SAFE_RELEASE(errors);

		hr = mDevice->getD3D11Device()->CreateVertexShader(static_cast<DWORD*>(microcode->GetBufferPointer()), 
			microcode->GetBufferSize(), mDevice->getClassLinkage(), &mClearQuadVS);

		if (FAILED(hr))
		{
			SAFE_RELEASE(microcode);
			CM_EXCEPT(RenderingAPIException, "Cannot create D3D11 high-level shader.");
		}

		// Create input layout
		D3D11_INPUT_ELEMENT_DESC* declElements = cm_newN<D3D11_INPUT_ELEMENT_DESC, ScratchAlloc>(2);
		ZeroMemory(declElements, sizeof(D3D11_INPUT_ELEMENT_DESC) * 2);

		declElements[0].SemanticName			= "POSITION";
		declElements[0].SemanticIndex			= 0;
		declElements[0].Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		declElements[0].InputSlot				= 0;
		declElements[0].AlignedByteOffset		= 0;
		declElements[0].InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		declElements[0].InstanceDataStepRate	= 0;

		declElements[1].SemanticName			= "COLOR";
		declElements[1].SemanticIndex			= 0;
		declElements[1].Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		declElements[1].InputSlot				= 0;
		declElements[1].AlignedByteOffset		= sizeof(float) * 3;
		declElements[1].InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		declElements[1].InstanceDataStepRate	= 0;

		hr = mDevice->getD3D11Device()->CreateInputLayout(declElements, 2, microcode->GetBufferPointer(), 
			microcode->GetBufferSize(), &mClearQuadIL);

		cm_deleteN<ScratchAlloc>(declElements, 2);

		if (FAILED(hr))
		{
			CM_EXCEPT(RenderingAPIException, "Cannot create D3D11 input layout.");
		}

		// Create vertex buffer
		D3D11_BUFFER_DESC mVBDesc;

		mVBDesc.ByteWidth = sizeof(float) * 3 + sizeof(UINT32);
		mVBDesc.MiscFlags = 0;
		mVBDesc.StructureByteStride = 0;

		mVBDesc.Usage = D3D11_USAGE_DEFAULT;
		mVBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
		mVBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		ClearVertex vertexData[4];
		vertexData[0].pos = Vector3(0, 0, 0);
		vertexData[1].pos = Vector3(0, 0, 0);
		vertexData[2].pos = Vector3(0, 0, 0);
		vertexData[3].pos = Vector3(0, 0, 0);

		vertexData[0].col = Color::White.getAsBGRA();
		vertexData[1].col = Color::White.getAsBGRA();
		vertexData[2].col = Color::White.getAsBGRA();
		vertexData[3].col = Color::White.getAsBGRA();

		D3D11_SUBRESOURCE_DATA vertexSubresourceData;
		vertexSubresourceData.pSysMem = vertexData;

		hr = mDevice->getD3D11Device()->CreateBuffer(&mVBDesc, &vertexSubresourceData, &mClearQuadVB);
		if (FAILED(hr) || mDevice->hasError())
		{
			String msg = mDevice->getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Cannot create D3D11 buffer: " + msg);
		}

		// Create index buffer
		D3D11_BUFFER_DESC mIBDesc;

		mIBDesc.ByteWidth = sizeof(UINT16) * 4;
		mIBDesc.MiscFlags = 0;
		mIBDesc.StructureByteStride = 0;

		mIBDesc.Usage = D3D11_USAGE_DEFAULT;
		mIBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
		mIBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		UINT16 indexData[6];
		indexData[0] = 0;
		indexData[1] = 1;
		indexData[2] = 2;
		indexData[3] = 1;
		indexData[4] = 3;
		indexData[5] = 2;

		D3D11_SUBRESOURCE_DATA indexSubresourceData;
		indexSubresourceData.pSysMem = indexData;

		hr = mDevice->getD3D11Device()->CreateBuffer(&mIBDesc, &indexSubresourceData, &mClearQuadIB);
		if (FAILED(hr) || mDevice->hasError())
		{
			String msg = mDevice->getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Cannot create D3D11 buffer: " + msg);
		}
	}
}