#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmHardwareVertexBuffer.h"

namespace CamelotEngine
{
	class VertexDeclarationST : public RTTIType<VertexDeclaration, IReflectable, VertexDeclarationST>
	{
	private:
		VertexElement& getElement(VertexDeclaration* obj, UINT32 idx)
		{
			auto iter = obj->mElementList.begin();
			for(int i = 0; i < idx; i++)
				++iter;

			return *iter;
		}

		void setElement(VertexDeclaration* obj, UINT32 idx, VertexElement& data)
		{
			auto iter = obj->mElementList.begin();
			for(int i = 0; i < idx; i++)
				++iter;

			*iter = data;
		}

		UINT32 getElementArraySize(VertexDeclaration* obj)
		{
			return obj->mElementList.size();
		}

		void setElementArraySize(VertexDeclaration* obj, UINT32 size)
		{
			for(int i = obj->mElementList.size(); i <= size; i++)
				obj->mElementList.push_back(VertexElement());
		}

	public:
		VertexDeclarationST()
		{
			addPlainArrayField("mElementList", 0, &VertexDeclarationST::getElement, &VertexDeclarationST::getElementArraySize, 
				&VertexDeclarationST::setElement, &VertexDeclarationST::setElementArraySize);
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{
			return std::shared_ptr<VertexDeclaration>(new VertexDeclaration());
		}

		virtual const String& getRTTIName() 
		{
			static String name = "VertexDeclaration";
			throw name;
		}

		virtual UINT32 getRTTIId() 
		{
			return TID_VertexDeclaration;
		}
	};
}