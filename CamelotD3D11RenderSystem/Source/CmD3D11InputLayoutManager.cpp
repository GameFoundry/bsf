#include "CmD3D11InputLayoutManager.h"
#include "CmD3D11Mappings.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"
#include "CmD3D11HLSLProgram.h"
#include "CmHardwareBufferManager.h"
#include "CmDebug.h"
#include "CmUtil.h"

namespace BansheeEngine
{
	size_t D3D11InputLayoutManager::HashFunc::operator()
		(const D3D11InputLayoutManager::VertexDeclarationKey &key) const
	{
		size_t hash = 0;
		hash_combine(hash, key.bufferDeclHash);
		hash_combine(hash, key.vertexProgramId);

		return hash;
	}

	bool D3D11InputLayoutManager::EqualFunc::operator()
		(const D3D11InputLayoutManager::VertexDeclarationKey &a, const D3D11InputLayoutManager::VertexDeclarationKey &b) const
		
	{
		if(a.bufferDeclElements->size() != b.bufferDeclElements->size())
			return false;

		{
			auto iter1 = a.bufferDeclElements->begin();
			auto iter2 = b.bufferDeclElements->begin();

			for(; iter1 != a.bufferDeclElements->end(); ++iter1, ++iter2)
			{
				if((*iter1) != (*iter2))
					return false;
			}
		}

		if(a.vertexProgramId != b.vertexProgramId)
			return false;

		return true;
	}

	D3D11InputLayoutManager::D3D11InputLayoutManager()
		:mLastUsedCounter(0), mWarningShown(false)
	{

	}

	D3D11InputLayoutManager::~D3D11InputLayoutManager()
	{
		while(mInputLayoutMap.begin() != mInputLayoutMap.end())
		{
			auto firstElem = mInputLayoutMap.begin();

			cm_delete<PoolAlloc>(firstElem->first.bufferDeclElements);
			SAFE_RELEASE(firstElem->second->inputLayout);
			cm_delete<PoolAlloc>(firstElem->second);

			mInputLayoutMap.erase(firstElem);
		}
	}

	ID3D11InputLayout* D3D11InputLayoutManager::retrieveInputLayout(VertexDeclarationPtr vertexShaderDecl, VertexDeclarationPtr vertexBufferDecl, D3D11HLSLProgram& vertexProgram)
	{
		VertexDeclarationKey pair;
		pair.bufferDeclHash = vertexBufferDecl->getHash();
		pair.bufferDeclElements = const_cast<List<VertexElement>::type*>(&vertexBufferDecl->getElements());
		pair.vertexProgramId = vertexProgram.getProgramId();

		auto iterFind = mInputLayoutMap.find(pair);
		if(iterFind == mInputLayoutMap.end())
		{
			if(mInputLayoutMap.size() >= DECLARATION_BUFFER_SIZE)
				removeLeastUsed(); // Prune so the buffer doesn't just infinitely grow

			addNewInputLayout(vertexShaderDecl, vertexBufferDecl, vertexProgram);

			iterFind = mInputLayoutMap.find(pair);

			if(iterFind == mInputLayoutMap.end()) // We failed to create input layout
				return nullptr;
		}

		iterFind->second->lastUsedIdx = ++mLastUsedCounter;
		return iterFind->second->inputLayout;
	}

	void D3D11InputLayoutManager::addNewInputLayout(VertexDeclarationPtr vertexShaderDecl, VertexDeclarationPtr vertexBufferDecl, D3D11HLSLProgram& vertexProgram)
	{
		if(!areCompatible(vertexShaderDecl, vertexBufferDecl))
			return; // Error was already reported, so just quit here

		UINT32 numElements = vertexBufferDecl->getElementCount();
		D3D11_INPUT_ELEMENT_DESC* declElements = cm_newN<D3D11_INPUT_ELEMENT_DESC, ScratchAlloc>(numElements);
		ZeroMemory(declElements, sizeof(D3D11_INPUT_ELEMENT_DESC) * numElements);

		unsigned int idx = 0;
		for(auto iter = vertexBufferDecl->getElements().begin(); iter != vertexBufferDecl->getElements().end(); ++iter)
		{
			declElements[idx].SemanticName			= D3D11Mappings::get(iter->getSemantic());
			declElements[idx].SemanticIndex			= iter->getSemanticIdx();
			declElements[idx].Format				= D3D11Mappings::get(iter->getType());
			declElements[idx].InputSlot				= iter->getStreamIdx();
			declElements[idx].AlignedByteOffset		= static_cast<WORD>(iter->getOffset());
			declElements[idx].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
			declElements[idx].InstanceDataStepRate	= 0;

			idx++;
		}

		D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = d3d11rs->getPrimaryDevice();

		const HLSLMicroCode& microcode = vertexProgram.getMicroCode();

		InputLayoutEntry* newEntry = cm_new<InputLayoutEntry, PoolAlloc>();
		newEntry->lastUsedIdx = ++mLastUsedCounter;
		newEntry->inputLayout = nullptr; 
		HRESULT hr = device.getD3D11Device()->CreateInputLayout( 
			declElements, 
			numElements, 
			&microcode[0], 
			microcode.size(),
			&newEntry->inputLayout);

		cm_deleteN<ScratchAlloc>(declElements, numElements);

		if (FAILED(hr)|| device.hasError())
			CM_EXCEPT(RenderingAPIException, "Unable to set D3D11 vertex declaration" + device.getErrorDescription());

		// Create key and add to the layout map
		VertexDeclarationKey pair;
		pair.bufferDeclHash = vertexBufferDecl->getHash();

		List<VertexElement>::type* bufferDeclElements = cm_new<List<VertexElement>::type, PoolAlloc>(); 
		pair.bufferDeclElements = bufferDeclElements;
		pair.vertexProgramId = vertexProgram.getProgramId();

		for(auto iter = vertexBufferDecl->getElements().begin(); iter != vertexBufferDecl->getElements().end(); ++iter)
			bufferDeclElements->push_back(*iter);

		mInputLayoutMap[pair] = newEntry;
	}

	void D3D11InputLayoutManager::removeLeastUsed()
	{
		if(!mWarningShown)
		{
			LOGWRN("Input layout buffer is full, pruning last " + toString(NUM_ELEMENTS_TO_PRUNE) + " elements. This is probably okay unless you are creating a massive amount of input layouts" \
				" as they will get re-created every frame. In that case you should increase the layout buffer size. This warning won't be shown again.");

			mWarningShown = true;
		}

		Map<UINT32, VertexDeclarationKey>::type leastFrequentlyUsedMap;

		for(auto iter = mInputLayoutMap.begin(); iter != mInputLayoutMap.end(); ++iter)
			leastFrequentlyUsedMap[iter->second->lastUsedIdx] = iter->first;


		UINT32 elemsRemoved = 0;
		for(auto iter = leastFrequentlyUsedMap.begin(); iter != leastFrequentlyUsedMap.end(); ++iter)
		{
			auto inputLayoutIter = mInputLayoutMap.find(iter->second);

			cm_delete<PoolAlloc>(inputLayoutIter->first.bufferDeclElements);
			SAFE_RELEASE(inputLayoutIter->second->inputLayout);
			cm_delete<PoolAlloc>(inputLayoutIter->second);

			mInputLayoutMap.erase(inputLayoutIter);

			elemsRemoved++;
			if(elemsRemoved >= NUM_ELEMENTS_TO_PRUNE)
				break;
		}
	}

	bool D3D11InputLayoutManager::areCompatible(VertexDeclarationPtr vertexShaderDecl, VertexDeclarationPtr vertexBufferDecl)
	{
		for(auto shaderIter = vertexShaderDecl->getElements().begin(); shaderIter != vertexShaderDecl->getElements().end(); ++shaderIter)
		{
			const VertexElement* foundElement = nullptr;
			for(auto bufferIter = vertexBufferDecl->getElements().begin(); bufferIter != vertexBufferDecl->getElements().end(); ++bufferIter)
			{
				if(shaderIter->getSemantic() == bufferIter->getSemantic() && shaderIter->getSemanticIdx() == bufferIter->getSemanticIdx())
				{
					foundElement = &(*bufferIter);
					break;
				}
			}

			if(foundElement == nullptr)
			{
				LOGWRN("Provided vertex buffer doesn't have a required input attribute: " + toString(shaderIter->getSemantic()) + toString(shaderIter->getSemanticIdx()));
				return false;
			}
		}

		return true;
	}
}