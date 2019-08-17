//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "RTTI/BsStringRTTI.h"
#include "RTTI/BsStdRTTI.h"
#include "RTTI/BsDataBlobRTTI.h"
#include "RenderAPI/BsGpuProgram.h"
#include "RenderAPI/BsGpuParamDesc.h"
#include "Managers/BsGpuProgramManager.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT GpuProgramBytecodeRTTI : public RTTIType<GpuProgramBytecode, IReflectable, GpuProgramBytecodeRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(instructions, 0)
			BS_RTTI_MEMBER_REFLPTR(paramDesc, 1)
			BS_RTTI_MEMBER_PLAIN(vertexInput, 2)
			BS_RTTI_MEMBER_PLAIN(messages, 3)
			BS_RTTI_MEMBER_PLAIN(compilerId, 4)
			BS_RTTI_MEMBER_PLAIN(compilerVersion, 5)
		BS_END_RTTI_MEMBERS

	public:
		const String& getRTTIName() override
		{
			static String name = "GpuProgramBytecode";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_GpuProgramBytecode;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<GpuProgramBytecode>();
		}
	};

	class BS_CORE_EXPORT GpuParamDescRTTI : public RTTIType<GpuParamDesc, IReflectable, GpuParamDescRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(paramBlocks, 0)
			BS_RTTI_MEMBER_PLAIN(params, 1)
			BS_RTTI_MEMBER_PLAIN(samplers, 2)
			BS_RTTI_MEMBER_PLAIN(textures, 3)
			BS_RTTI_MEMBER_PLAIN(loadStoreTextures, 4)
			BS_RTTI_MEMBER_PLAIN(buffers, 5)
		BS_END_RTTI_MEMBERS

	public:
		const String& getRTTIName() override
		{
			static String name = "GpuParamDesc";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_GpuParamDesc;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<GpuParamDesc>();
		}
	};

	class BS_CORE_EXPORT GpuProgramRTTI : public RTTIType<GpuProgram, IReflectable, GpuProgramRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(mType, 2)
			BS_RTTI_MEMBER_PLAIN(mNeedsAdjacencyInfo, 3)
			BS_RTTI_MEMBER_PLAIN(mEntryPoint, 4)
			BS_RTTI_MEMBER_PLAIN(mSource, 6)
			BS_RTTI_MEMBER_PLAIN(mLanguage, 7)
		BS_END_RTTI_MEMBERS

	public:
		void onSerializationStarted(IReflectable* obj, SerializationContext* context) override
		{
			// Need to ensure the core thread object is initialized
			GpuProgram* gpuProgram = static_cast<GpuProgram*>(obj);
			gpuProgram->blockUntilCoreInitialized();
		}

		void onDeserializationEnded(IReflectable* obj, SerializationContext* context) override
		{
			GpuProgram* gpuProgram = static_cast<GpuProgram*>(obj);
			gpuProgram->initialize();
		}

		const String& getRTTIName() override
		{
			static String name = "GpuProgram";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_GpuProgram;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return GpuProgramManager::instance().createEmpty("", GPT_VERTEX_PROGRAM); // Params don't matter, they'll get overwritten
		}
	};

	template<> struct RTTIPlainType<GpuParamDataDesc>
	{
		enum { id = TID_GpuParamDataDesc }; enum { hasDynamicSize = 1 };
		static constexpr UINT32 VERSION = 1;

		static void toMemory(const GpuParamDataDesc& data, char* memory)
		{
			UINT32 size = getDynamicSize(data);

			UINT32 curSize = sizeof(UINT32);
			memcpy(memory, &size, curSize);
			memory += curSize;

			memory = rttiWriteElem(VERSION, memory);

			memory = rttiWriteElem(data.name, memory);
			memory = rttiWriteElem(data.elementSize, memory);
			memory = rttiWriteElem(data.arraySize, memory);
			memory = rttiWriteElem(data.arrayElementStride, memory);
			memory = rttiWriteElem(data.type, memory);

			memory = rttiWriteElem(data.paramBlockSlot, memory);
			memory = rttiWriteElem(data.paramBlockSet, memory);
			memory = rttiWriteElem(data.gpuMemOffset, memory);
			rttiWriteElem(data.cpuMemOffset, memory);
		}

		static UINT32 fromMemory(GpuParamDataDesc& data, char* memory)
		{
			UINT32 size;
			memcpy(&size, memory, sizeof(UINT32));
			memory += sizeof(UINT32);

			UINT32 version = 0;
			memory = rttiReadElem(version, memory);
			assert(version == VERSION);

			memory = rttiReadElem(data.name, memory);
			memory = rttiReadElem(data.elementSize, memory);
			memory = rttiReadElem(data.arraySize, memory);
			memory = rttiReadElem(data.arrayElementStride, memory);
			memory = rttiReadElem(data.type, memory);

			memory = rttiReadElem(data.paramBlockSlot, memory);
			memory = rttiReadElem(data.paramBlockSet, memory);
			memory = rttiReadElem(data.gpuMemOffset, memory);
			rttiReadElem(data.cpuMemOffset, memory);

			return size;
		}

		static UINT32 getDynamicSize(const GpuParamDataDesc& data)
		{
			UINT32 dataSize = rttiGetElemSize(VERSION) + rttiGetElemSize(data.name) + rttiGetElemSize(data.elementSize) +
				rttiGetElemSize(data.arraySize) + rttiGetElemSize(data.arrayElementStride) + rttiGetElemSize(data.type) +
				rttiGetElemSize(data.paramBlockSlot) + rttiGetElemSize(data.paramBlockSet) +
				rttiGetElemSize(data.gpuMemOffset) + rttiGetElemSize(data.cpuMemOffset) + sizeof(UINT32);

			return dataSize;
		}
	};

	template<> struct RTTIPlainType<GpuParamObjectDesc>
	{
		enum { id = TID_GpuParamObjectDesc }; enum { hasDynamicSize = 1 };
		static constexpr UINT32 VERSION = 2;

		static void toMemory(const GpuParamObjectDesc& data, char* memory)
		{
			UINT32 size = getDynamicSize(data);

			UINT32 curSize = sizeof(UINT32);
			memcpy(memory, &size, curSize);
			memory += curSize;

			memory = rttiWriteElem(VERSION, memory);

			memory = rttiWriteElem(data.name, memory);
			memory = rttiWriteElem(data.type, memory);
			memory = rttiWriteElem(data.slot, memory);
			memory = rttiWriteElem(data.set, memory);
			rttiWriteElem(data.elementType, memory);
		}

		static UINT32 fromMemory(GpuParamObjectDesc& data, char* memory)
		{
			UINT32 size;
			memcpy(&size, memory, sizeof(UINT32));
			memory += sizeof(UINT32);

			UINT32 version = 0;
			memory = rttiReadElem(version, memory);

			memory = rttiReadElem(data.name, memory);
			memory = rttiReadElem(data.type, memory);
			memory = rttiReadElem(data.slot, memory);
			memory = rttiReadElem(data.set, memory);

			if (version > 1)
				rttiReadElem(data.elementType, memory);

			return size;
		}

		static UINT32 getDynamicSize(const GpuParamObjectDesc& data)
		{
			UINT32 dataSize = rttiGetElemSize(VERSION) + rttiGetElemSize(data.name) + rttiGetElemSize(data.type) +
				rttiGetElemSize(data.slot) + rttiGetElemSize(data.set) + rttiGetElemSize(data.elementType) + sizeof(UINT32);

			return dataSize;
		}
	};

	template<> struct RTTIPlainType<GpuParamBlockDesc>
	{
		enum { id = TID_GpuParamBlockDesc }; enum { hasDynamicSize = 1 };
		static constexpr UINT32 VERSION = 1;

		static void toMemory(const GpuParamBlockDesc& data, char* memory)
		{
			UINT32 size = getDynamicSize(data);

			UINT32 curSize = sizeof(UINT32);
			memcpy(memory, &size, curSize);
			memory += curSize;

			memory = rttiWriteElem(VERSION, memory);

			memory = rttiWriteElem(data.name, memory);
			memory = rttiWriteElem(data.set, memory);
			memory = rttiWriteElem(data.slot, memory);
			memory = rttiWriteElem(data.blockSize, memory);
			rttiWriteElem(data.isShareable, memory);
		}

		static UINT32 fromMemory(GpuParamBlockDesc& data, char* memory)
		{
			UINT32 size;
			memcpy(&size, memory, sizeof(UINT32));
			memory += sizeof(UINT32);

			UINT32 version = 0;
			memory = rttiReadElem(version, memory);
			assert(version == VERSION);

			memory = rttiReadElem(data.name, memory);
			memory = rttiReadElem(data.set, memory);
			memory = rttiReadElem(data.slot, memory);
			memory = rttiReadElem(data.blockSize, memory);
			rttiReadElem(data.isShareable, memory);

			return size;
		}

		static UINT32 getDynamicSize(const GpuParamBlockDesc& data)
		{
			UINT32 dataSize = rttiGetElemSize(VERSION) + rttiGetElemSize(data.name) + rttiGetElemSize(data.set) +
				rttiGetElemSize(data.slot) + rttiGetElemSize(data.blockSize) + rttiGetElemSize(data.isShareable) +
				sizeof(UINT32);

			return dataSize;
		}
	};

	/** @} */
	/** @endcond */
}
