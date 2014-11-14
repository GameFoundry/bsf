#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsVertexBuffer.h"
#include "BsHardwareBufferManager.h"

namespace BansheeEngine
{
	class VertexDeclarationRTTI : public RTTIType<VertexDeclaration, IReflectable, VertexDeclarationRTTI>
	{
	private:
		VertexElement& getElement(VertexDeclaration* obj, UINT32 idx)
		{
			List<VertexElement>& elemList = obj->mProperties.mElementList;

			auto iter = elemList.begin();
			for(UINT32 i = 0; i < idx; i++)
				++iter;

			return *iter;
		}

		void setElement(VertexDeclaration* obj, UINT32 idx, VertexElement& data)
		{
			List<VertexElement>& elemList = obj->mProperties.mElementList;

			auto iter = elemList.begin();
			for(UINT32 i = 0; i < idx; i++)
				++iter;

			*iter = data;
		}

		UINT32 getElementArraySize(VertexDeclaration* obj)
		{
			List<VertexElement>& elemList = obj->mProperties.mElementList;

			return (UINT32)elemList.size();
		}

		void setElementArraySize(VertexDeclaration* obj, UINT32 size)
		{
			List<VertexElement>& elemList = obj->mProperties.mElementList;

			for (size_t i = elemList.size(); i < size; i++)
				elemList.push_back(VertexElement());
		}

	public:
		VertexDeclarationRTTI()
		{
			addPlainArrayField("mElementList", 0, &VertexDeclarationRTTI::getElement, &VertexDeclarationRTTI::getElementArraySize, 
				&VertexDeclarationRTTI::setElement, &VertexDeclarationRTTI::setElementArraySize);
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{
			return HardwareBufferManager::instance().createVertexDeclaration(List<VertexElement>());
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