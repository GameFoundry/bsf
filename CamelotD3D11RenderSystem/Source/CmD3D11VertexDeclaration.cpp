#include "CmD3D11VertexDeclaration.h"
#include "CmD3D11Mappings.h"
#include "CmD3D11Device.h"
#include "CmD3D11HLSLProgram.h"
#include "CmException.h"

namespace CamelotEngine
{
	D3D11VertexDeclaration::D3D11VertexDeclaration()
		:VertexDeclaration()
	{
	}

	D3D11VertexDeclaration::~D3D11VertexDeclaration()
	{
		for(auto iter = mLayoutPerDevice.begin(); iter != mLayoutPerDevice.end(); ++iter)
		{
			if(iter->second != nullptr)
				iter->second->Release();
		}
	}

	const VertexElement& D3D11VertexDeclaration::addElement(unsigned short source, UINT32 offset, VertexElementType theType,
		VertexElementSemantic semantic, unsigned short index)
	{
		mNeedsRebuild = true;
		return VertexDeclaration::addElement(source, offset, theType, semantic, index);
	}

	const VertexElement& D3D11VertexDeclaration::insertElement(unsigned short atPosition,
		unsigned short source, UINT32 offset, VertexElementType theType,
		VertexElementSemantic semantic, unsigned short index)
	{
		mNeedsRebuild = true;
		return VertexDeclaration::insertElement(atPosition, source, offset, theType, semantic, index);
	}

	void D3D11VertexDeclaration::removeElement(unsigned short elem_index)
	{
		VertexDeclaration::removeElement(elem_index);
		mNeedsRebuild = true;
	}

	void D3D11VertexDeclaration::removeElement(VertexElementSemantic semantic, unsigned short index)
	{
		VertexDeclaration::removeElement(semantic, index);
		mNeedsRebuild = true;
	}

	void D3D11VertexDeclaration::removeAllElements(void)
	{
		VertexDeclaration::removeAllElements();
		mNeedsRebuild = true;
	}

	void D3D11VertexDeclaration::modifyElement(unsigned short elem_index, unsigned short source, UINT32 offset, VertexElementType theType,
		VertexElementSemantic semantic, unsigned short index)
	{
		VertexDeclaration::modifyElement(elem_index, source, offset, theType, semantic, index);
		mNeedsRebuild = true;
	}

	ID3D11InputLayout* D3D11VertexDeclaration::getD3DLayout(D3D11Device& device, D3D11HLSLProgram& programToBindTo)
	{
		if(!mNeedsRebuild)
		{
			auto iterFind = mLayoutPerDevice.find(&device);

			if(iterFind == mLayoutPerDevice.end())
				mNeedsRebuild = true;
		}

		if(mNeedsRebuild)
		{
			size_t numElements = programToBindTo.getNumInputs();

			D3D11_INPUT_ELEMENT_DESC* declElements = new D3D11_INPUT_ELEMENT_DESC[numElements];
			ZeroMemory(declElements, sizeof(D3D11_INPUT_ELEMENT_DESC) * numElements);

			unsigned int idx;
			for (idx = 0; idx < numElements; ++idx)
			{
				const D3D11_SIGNATURE_PARAMETER_DESC& inputDesc = programToBindTo.getInputParamDesc(idx);
				VertexElementList::const_iterator i, iend;
				iend = mElementList.end();
				bool found = false;
				for (i = mElementList.begin(); i != iend; ++i)
				{
					LPCSTR semanticName			= D3D11Mappings::get(i->getSemantic());
					UINT semanticIndex			= i->getIndex();
					if(strcmp(semanticName, inputDesc.SemanticName) == 0
						&& semanticIndex == inputDesc.SemanticIndex)
					{
						found = true;
						break;
					}
				}

				if(!found)
				{
					CM_EXCEPT(RenderingAPIException, "Shader signature doesn't match the vertex declaration!");
				}

				declElements[idx].SemanticName			= inputDesc.SemanticName;
				declElements[idx].SemanticIndex			= inputDesc.SemanticIndex;
				declElements[idx].Format				= D3D11Mappings::get(i->getType());
				declElements[idx].InputSlot				= i->getSource();
				declElements[idx].AlignedByteOffset		= static_cast<WORD>(i->getOffset());
				declElements[idx].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
				declElements[idx].InstanceDataStepRate	= 0;
				
				DWORD dwShaderFlags = 0;
				const HLSLMicroCode& microCode = programToBindTo.getMicroCode();

				ID3D11InputLayout* inputLayout = nullptr; 
				HRESULT hr = device.getD3D11Device()->CreateInputLayout( 
					declElements, 
					programToBindTo.getNumInputs(), 
					&microCode[0], 
					microCode.size(),
					&inputLayout );

				if (FAILED(hr)|| device.hasError())
				{
					String errorDescription = device.getErrorDescription();

					CM_EXCEPT(RenderingAPIException, "Unable to set D3D11 vertex declaration" + errorDescription);
				}

				mLayoutPerDevice[&device] = inputLayout;

			}
		}

		return mLayoutPerDevice[&device];
	}
}