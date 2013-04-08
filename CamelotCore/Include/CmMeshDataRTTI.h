#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmMeshData.h"
#include "CmManagedDataBlock.h"
#include "CmVertexDeclaration.h"

namespace CamelotEngine
{
	class CM_EXPORT VertexElementDataRTTI : public RTTIType<MeshData::VertexElementData, IReflectable, VertexElementDataRTTI>
	{
	private:
		ManagedDataBlock getVertexData(MeshData::VertexElementData* obj) { return ManagedDataBlock(obj->data, obj->elementCount * obj->element.getSize(), false); }	
		void setVertexData(MeshData::VertexElementData* obj, ManagedDataBlock val) { obj->data = val.getData(); } 

		UINT32& getNumElements(MeshData::VertexElementData* obj) { return obj->elementCount; }
		void setNumElements(MeshData::VertexElementData* obj, UINT32& value) { obj->elementCount = value; }

		VertexElement& getVertexElement(MeshData::VertexElementData* obj) { return obj->element; }
		void setVertexElement(MeshData::VertexElementData* obj, VertexElement& value) { obj->element = value; }

	public:
		VertexElementDataRTTI()
		{
			addDataBlockField("data", 0, &VertexElementDataRTTI::getVertexData, &VertexElementDataRTTI::setVertexData);
			addPlainField("elementCount", 1, &VertexElementDataRTTI::getNumElements, &VertexElementDataRTTI::setNumElements);
			addPlainField("element", 2, &VertexElementDataRTTI::getVertexElement, &VertexElementDataRTTI::setVertexElement);
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{
			return std::shared_ptr<MeshData::VertexElementData>(new MeshData::VertexElementData());
		}

		virtual const String& getRTTIName() 
		{
			static String name = "VertexElementData";
			throw name;
		}

		virtual UINT32 getRTTIId() 
		{
			return TID_VertexElementData;
		}
	};

	class CM_EXPORT IndexElementDataRTTI : public RTTIType<MeshData::IndexElementData, IReflectable, IndexElementDataRTTI>
	{
	private:
		ManagedDataBlock getIndexData(MeshData::IndexElementData* obj) { return ManagedDataBlock(obj->indices, obj->numIndices * obj->elementSize, false); }	
		void setIndexData(MeshData::IndexElementData* obj, ManagedDataBlock val) { obj->indices = val.getData(); } 

		UINT32& getNumIndices(MeshData::IndexElementData* obj) { return obj->numIndices; }
		void setNumIndices(MeshData::IndexElementData* obj, UINT32& value) { obj->numIndices = value; }

		UINT32& getElementSize(MeshData::IndexElementData* obj) { return obj->elementSize; }
		void setElementSize(MeshData::IndexElementData* obj, UINT32& value) { obj->elementSize = value; }

		UINT32& getSubMesh(MeshData::IndexElementData* obj) { return obj->subMesh; }
		void setSubMesh(MeshData::IndexElementData* obj, UINT32& value) { obj->subMesh = value; }
	public:
		IndexElementDataRTTI()
		{
			addDataBlockField("indices", 0, &IndexElementDataRTTI::getIndexData, &IndexElementDataRTTI::setIndexData);
			addPlainField("numIndices", 1, &IndexElementDataRTTI::getNumIndices, &IndexElementDataRTTI::setNumIndices);
			addPlainField("elementSize", 2, &IndexElementDataRTTI::getElementSize, &IndexElementDataRTTI::setElementSize);
			addPlainField("subMesh", 3, &IndexElementDataRTTI::getSubMesh, &IndexElementDataRTTI::setSubMesh);
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{
			return std::shared_ptr<MeshData::IndexElementData>(new MeshData::IndexElementData());
		}

		virtual const String& getRTTIName() 
		{
			static String name = "IndexElementData";
			throw name;
		}

		virtual UINT32 getRTTIId() 
		{
			return TID_IndexElementData;
		}
	};

	CM_ALLOW_MEMCPY_SERIALIZATION(IndexBuffer::IndexType);

	class CM_EXPORT MeshDataRTTI : public RTTIType<MeshData, IReflectable, MeshDataRTTI>
	{
	private:
		struct TempMeshData
		{
			vector<MeshData::VertexElementData>::type vertexElements;
		};

		MeshData::VertexElementData& getVertexElementData(MeshData* obj, UINT32 arrayIdx)
		{
			auto tempData = boost::any_cast<std::shared_ptr<TempMeshData>>(obj->mRTTIData);
			return tempData->vertexElements[arrayIdx];
		}

		void setVertexElementData(MeshData* obj, UINT32 arrayIdx, MeshData::VertexElementData& value)
		{
			obj->clearIfItExists(value.element.getType(), value.element.getSemantic(), value.element.getIndex(), value.element.getSource());

			obj->mVertexData[value.element.getSource()].push_back(value);
		}

		UINT32 getNumVertexElementData(MeshData* obj)
		{
			auto tempData = boost::any_cast<std::shared_ptr<TempMeshData>>(obj->mRTTIData);
			return (UINT32)tempData->vertexElements.size();
		}

		void setNumVertexElementData(MeshData* obj, UINT32 numElements)
		{
			// Do nothing
		}

		MeshData::IndexElementData& getIndexElementData(MeshData* obj, UINT32 arrayIdx)
		{
			return obj->mIndices[arrayIdx];
		}

		void setIndexElementData(MeshData* obj, UINT32 arrayIdx, MeshData::IndexElementData& value)
		{
			obj->mIndices[arrayIdx] = value;
		}

		UINT32 getNumIndexElementData(MeshData* obj)
		{
			return (UINT32)obj->mIndices.size();
		}

		void setNumIndexElementData(MeshData* obj, UINT32 numElements)
		{
			obj->mIndices.resize(numElements);
		}

		IndexBuffer::IndexType& getIndexType(MeshData* obj)
		{
			return obj->mIndexType;
		}

		void setIndexType(MeshData* obj, IndexBuffer::IndexType& value)
		{
			obj->mIndexType = value;
		}

	public:
		MeshDataRTTI()
		{
			addReflectableArrayField("mVertexData", 0, &MeshDataRTTI::getVertexElementData, 
				&MeshDataRTTI::getNumVertexElementData, &MeshDataRTTI::setVertexElementData, &MeshDataRTTI::setNumVertexElementData);

			addReflectableArrayField("mIndexBuffer", 1, &MeshDataRTTI::getIndexElementData, 
				&MeshDataRTTI::getNumIndexElementData, &MeshDataRTTI::setIndexElementData, &MeshDataRTTI::setNumIndexElementData);

			addPlainField("mIndexType", 2, &MeshDataRTTI::getIndexType, &MeshDataRTTI::setIndexType);
		}

		virtual void onSerializationStarted(IReflectable* obj);
		virtual void onSerializationEnded(IReflectable* obj);

		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{
			return std::shared_ptr<MeshData>(new MeshData());
		}

		virtual const String& getRTTIName() 
		{
			static String name = "MeshData";
			throw name;
		}

		virtual UINT32 getRTTIId() 
		{
			return TID_MeshData;
		}
	};
}