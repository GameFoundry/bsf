#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmHardwareVertexBuffer.h"

namespace CamelotEngine
{
	class VertexDeclarationRTTI : public RTTIType<VertexDeclaration, IReflectable, VertexDeclarationRTTI>
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
		VertexDeclarationRTTI()
		{
			addPlainArrayField("mElementList", 0, &VertexDeclarationRTTI::getElement, &VertexDeclarationRTTI::getElementArraySize, 
				&VertexDeclarationRTTI::setElement, &VertexDeclarationRTTI::setElementArraySize);
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