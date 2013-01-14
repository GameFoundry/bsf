#include "CmD3D11InputLayoutManager.h"
#include "CmD3D11Mappings.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"
#include "CmD3D11HLSLProgram.h"
#include "CmHardwareBufferManager.h"
#include "CmDebug.h"
#include "CmUtil.h"

namespace CamelotEngine
{
	size_t D3D11InputLayoutManager::HashFunc::operator()
		(const D3D11InputLayoutManager::VertexDeclarationPair &key) const
	{
		size_t hash = 0;
		hash_combine(hash, key.bufferDeclHash);
		hash_combine(hash, key.shaderDeclHash);

		return hash;
	}

	bool D3D11InputLayoutManager::EqualFunc::operator()
		(const D3D11InputLayoutManager::VertexDeclarationPair &a, const D3D11InputLayoutManager::VertexDeclarationPair &b) const
		
	{
		if(a.bufferDeclElements->size() != b.bufferDeclElements->size())
			return false;

		if(a.shaderDeclElements->size() != b.shaderDeclElements->size())
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

		{
			auto iter1 = a.shaderDeclElements->begin();
			auto iter2 = b.shaderDeclElements->begin();

			for(; iter1 != a.shaderDeclElements->end(); ++iter1, ++iter2)
			{
				if((*iter1) != (*iter2))
					return false;
			}
		}

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

			delete firstElem->first.bufferDeclElements;
			delete firstElem->first.shaderDeclElements;
			SAFE_RELEASE(firstElem->second->inputLayout);
			delete firstElem->second;

			mInputLayoutMap.erase(firstElem);
		}
	}

	ID3D11InputLayout* D3D11InputLayoutManager::retrieveInputLayout(VertexDeclarationPtr vertexShaderDecl, VertexDeclarationPtr vertexBufferDecl, D3D11HLSLProgram& vertexProgram)
	{
		VertexDeclarationPair pair;
		pair.shaderDeclHash = vertexShaderDecl->getHash();
		pair.bufferDeclHash = vertexBufferDecl->getHash();
		pair.shaderDeclElements = &vertexShaderDecl->getElements();
		pair.bufferDeclElements = &vertexBufferDecl->getElements();

		auto iterFind = mInputLayoutMap.find(pair);
		if(iterFind == mInputLayoutMap.end())
		{
			if(mInputLayoutMap.size() >= DECLARATION_BUFFER_SIZE)
				removeLeastUsed(); // Prune so the buffer doesn't just infinitely grow

			addNewInputLayout(vertexShaderDecl, vertexBufferDecl, vertexProgram.getMicroCode());

			iterFind = mInputLayoutMap.find(pair);

			if(iterFind == mInputLayoutMap.end()) // We failed to create input layout
				return nullptr;
		}

		iterFind->second->lastUsedIdx = ++mLastUsedCounter;
		return iterFind->second->inputLayout;
	}

	void D3D11InputLayoutManager::addNewInputLayout(VertexDeclarationPtr vertexShaderDecl, VertexDeclarationPtr vertexBufferDecl, const HLSLMicroCode& microCode)
	{
		// Combine declarations and create input layout
		VertexDeclarationPtr newDecl = createCombinedDesc(vertexShaderDecl, vertexBufferDecl);

		if(newDecl == nullptr) // Error was already handled, so just quit here
			return;

		UINT32 numElements = newDecl->getElementCount();
		D3D11_INPUT_ELEMENT_DESC* declElements = new D3D11_INPUT_ELEMENT_DESC[numElements];
		ZeroMemory(declElements, sizeof(D3D11_INPUT_ELEMENT_DESC) * numElements);

		unsigned int idx;
		for(auto iter = newDecl->getElements().begin(); iter != newDecl->getElements().end(); ++iter)
		{
			declElements[idx].SemanticName			= D3D11Mappings::get(iter->getSemantic());
			declElements[idx].SemanticIndex			= iter->getIndex();
			declElements[idx].Format				= D3D11Mappings::get(iter->getType());
			declElements[idx].InputSlot				= iter->getSource();
			declElements[idx].AlignedByteOffset		= static_cast<WORD>(iter->getOffset());
			declElements[idx].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
			declElements[idx].InstanceDataStepRate	= 0;

			idx++;
		}

		D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = d3d11rs->getPrimaryDevice();

		InputLayoutEntry* newEntry = new InputLayoutEntry();
		newEntry->lastUsedIdx = ++mLastUsedCounter;
		newEntry->inputLayout = nullptr; 
		HRESULT hr = device.getD3D11Device()->CreateInputLayout( 
			declElements, 
			numElements, 
			&microCode[0], 
			microCode.size(),
			&newEntry->inputLayout);

		if (FAILED(hr)|| device.hasError())
			CM_EXCEPT(RenderingAPIException, "Unable to set D3D11 vertex declaration" + device.getErrorDescription());

		// Create key and add to the layout map
		VertexDeclarationPair pair;
		pair.shaderDeclHash = vertexShaderDecl->getHash();
		pair.bufferDeclHash = vertexBufferDecl->getHash();

		list<VertexElement>::type* shaderDeclElements = new list<VertexElement>::type(); 
		list<VertexElement>::type* bufferDeclElements = new list<VertexElement>::type(); 
		pair.shaderDeclElements = shaderDeclElements; 
		pair.bufferDeclElements = bufferDeclElements;

		for(auto iter = vertexShaderDecl->getElements().begin(); iter != vertexShaderDecl->getElements().end(); ++iter)
			shaderDeclElements->push_back(*iter);

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

		map<UINT32, VertexDeclarationPair>::type leastFrequentlyUsedMap;

		for(auto iter = mInputLayoutMap.begin(); iter != mInputLayoutMap.end(); ++iter)
			leastFrequentlyUsedMap[iter->second->lastUsedIdx] = iter->first;


		UINT32 elemsRemoved = 0;
		for(auto iter = leastFrequentlyUsedMap.begin(); iter != leastFrequentlyUsedMap.end(); ++iter)
		{
			auto inputLayoutIter = mInputLayoutMap.find(iter->second);

			delete inputLayoutIter->first.bufferDeclElements;
			delete inputLayoutIter->first.shaderDeclElements;
			SAFE_RELEASE(inputLayoutIter->second->inputLayout);
			delete inputLayoutIter->second;

			mInputLayoutMap.erase(inputLayoutIter);

			elemsRemoved++;
			if(elemsRemoved >= NUM_ELEMENTS_TO_PRUNE)
				break;
		}
	}

	VertexDeclarationPtr D3D11InputLayoutManager::createCombinedDesc(VertexDeclarationPtr vertexShaderDecl, VertexDeclarationPtr vertexBufferDecl)
	{
		VertexDeclarationPtr newDecl = HardwareBufferManager::instance().createVertexDeclaration();

		for(auto shaderIter = vertexShaderDecl->getElements().begin(); shaderIter != vertexShaderDecl->getElements().end(); ++shaderIter)
		{
			const VertexElement* foundElement = nullptr;
			for(auto bufferIter = vertexBufferDecl->getElements().begin(); bufferIter != vertexBufferDecl->getElements().end(); ++bufferIter)
			{
				if(shaderIter->getSemantic() == bufferIter->getSemantic() && shaderIter->getIndex() == bufferIter->getSemantic())
				{
					foundElement = &(*bufferIter);
					break;
				}
			}

			if(foundElement == nullptr)
			{
				// TODO - It's possible I can just skip this attribute and driver won't complain?
				LOGWRN("Provided vertex buffer doesn't have a required input attribute: " + toString(shaderIter->getSemantic()) + toString(shaderIter->getIndex()));
				return nullptr;
			}

			if(foundElement->getType() != shaderIter->getType())
			{
				// TODO - It's possible I can just cast to the smaller size and driver won't complain?
				LOGWRN("Shader input and vertex buffer types don't match for element with semantic: " + toString(shaderIter->getSemantic()) + toString(shaderIter->getIndex()) + ". " \
					"Buffer type is " + toString(foundElement->getType()) + " and shader type is " + toString(shaderIter->getType()));
				return nullptr;
			}

			newDecl->addElement(foundElement->getSource(), foundElement->getOffset(), foundElement->getType(), foundElement->getSemantic(), foundElement->getIndex());
		}

		return newDecl;
	}
}