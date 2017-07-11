//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsLightProbeVolume.h"
#include "BsLightProbeVolumeRTTI.h"
#include "BsFrameAlloc.h"
#include "BsRenderer.h"
#include "BsLight.h"
#include <atlalloc.h>

namespace bs
{
	LightProbeVolumeBase::LightProbeVolumeBase()
		: mPosition(BsZero), mRotation(BsIdentity), mIsActive(true)
	{ }

	LightProbeVolume::LightProbeVolume()
	{ }

	LightProbeVolume::LightProbeVolume(const AABox& volume, const Vector3& density)
	{
		// TODO - Generates probes in the grid volume
	}

	UINT32 LightProbeVolume::addProbe(const Vector3& position)
	{
		UINT32 handle = mNextProbeId++;
		mProbes[handle] = ProbeInfo(LightProbeFlags::Dirty, position);

		_markCoreDirty();
		return handle;
	}

	void LightProbeVolume::removeProbe(UINT32 handle)
	{
		auto iterFind = mProbes.find(handle);
		if (iterFind != mProbes.end() && mProbes.size() > 4)
		{
			iterFind->second.flags = LightProbeFlags::Removed;
			_markCoreDirty();
		}
	}

	void LightProbeVolume::setProbePosition(UINT32 handle, const Vector3& position)
	{
		auto iterFind = mProbes.find(handle);
		if (iterFind != mProbes.end())
		{
			iterFind->second.flags = LightProbeFlags::Dirty;
			iterFind->second.position = position;
			_markCoreDirty();
		}
	}

	Vector3 LightProbeVolume::getProbePosition(UINT32 handle) const
	{
		auto iterFind = mProbes.find(handle);
		if (iterFind != mProbes.end())
			return iterFind->second.position;
		
		return Vector3::ZERO;
	}

	SPtr<ct::LightProbeVolume> LightProbeVolume::getCore() const
	{
		return std::static_pointer_cast<ct::LightProbeVolume>(mCoreSpecific);
	}

	SPtr<LightProbeVolume> LightProbeVolume::create(const AABox& volume, const Vector3& density)
	{
		LightProbeVolume* probeVolume = new (bs_alloc<LightProbeVolume>()) LightProbeVolume(volume, density);
		SPtr<LightProbeVolume> probeVolumePtr = bs_core_ptr<LightProbeVolume>(probeVolume);
		probeVolumePtr->_setThisPtr(probeVolumePtr);
		probeVolumePtr->initialize();

		return probeVolumePtr;
	}

	SPtr<LightProbeVolume> LightProbeVolume::createEmpty()
	{
		LightProbeVolume* probeVolume = new (bs_alloc<LightProbeVolume>()) LightProbeVolume();
		SPtr<LightProbeVolume> probleVolumePtr = bs_core_ptr<LightProbeVolume>(probeVolume);
		probleVolumePtr->_setThisPtr(probleVolumePtr);

		return probleVolumePtr;
	}

	SPtr<ct::CoreObject> LightProbeVolume::createCore() const
	{
		ct::LightProbeVolume* handler = new (bs_alloc<ct::LightProbeVolume>()) ct::LightProbeVolume(mProbes);
		SPtr<ct::LightProbeVolume> handlerPtr = bs_shared_ptr<ct::LightProbeVolume>(handler);
		handlerPtr->_setThisPtr(handlerPtr);

		return handlerPtr;
	}

	CoreSyncData LightProbeVolume::syncToCore(FrameAlloc* allocator)
	{
		UINT32 size = 0;
		UINT8* buffer = nullptr;

		bs_frame_mark();
		{
			FrameVector<std::pair<UINT32, ProbeInfo>> dirtyProbes;
			FrameVector<UINT32> removedProbes;
			for (auto& probe : mProbes)
			{
				if (probe.second.flags == LightProbeFlags::Dirty)
				{
					dirtyProbes.push_back(std::make_pair(probe.first, probe.second));
					probe.second.flags = LightProbeFlags::Clean;
				}
				else if (probe.second.flags == LightProbeFlags::Removed)
				{
					removedProbes.push_back(probe.first);
					probe.second.flags = LightProbeFlags::Empty;
				}
			}

			UINT32 numDirtyProbes = (UINT32)dirtyProbes.size();
			UINT32 numRemovedProbes = (UINT32)removedProbes.size();

			size += rttiGetElemSize(mPosition);
			size += rttiGetElemSize(mRotation);
			size += rttiGetElemSize(mIsActive);
			size += rttiGetElemSize(numDirtyProbes);
			size += rttiGetElemSize(numRemovedProbes);
			size += (sizeof(UINT32) + sizeof(Vector3) + sizeof(LightProbeFlags)) * numDirtyProbes;
			size += sizeof(UINT32) * numRemovedProbes;

			buffer = allocator->alloc(size);

			char* dataPtr = (char*)buffer;
			dataPtr = rttiWriteElem(mPosition, dataPtr);
			dataPtr = rttiWriteElem(mRotation, dataPtr);
			dataPtr = rttiWriteElem(mIsActive, dataPtr);
			dataPtr = rttiWriteElem(numDirtyProbes, dataPtr);
			dataPtr = rttiWriteElem(numRemovedProbes, dataPtr);

			for (auto& entry : dirtyProbes)
			{
				dataPtr = rttiWriteElem(entry.first, dataPtr);
				dataPtr = rttiWriteElem(entry.second.position, dataPtr);
				dataPtr = rttiWriteElem(entry.second.flags, dataPtr);
			}

			for(auto& entry : removedProbes)
				dataPtr = rttiWriteElem(entry, dataPtr);
		}
		bs_frame_clear();

		return CoreSyncData(buffer, size);
	}

	void LightProbeVolume::_markCoreDirty()
	{
		markCoreDirty();
	}

	RTTITypeBase* LightProbeVolume::getRTTIStatic()
	{
		return LightProbeVolumeRTTI::instance();
	}

	RTTITypeBase* LightProbeVolume::getRTTI() const
	{
		return LightProbeVolume::getRTTIStatic();
	}

	namespace ct
	{
	LightProbeVolume::LightProbeVolume(const UnorderedMap<UINT32, bs::LightProbeVolume::ProbeInfo>& probes)
	{
		UINT32 probeIdx = 0;
		for(auto& entry : probes)
		{
			mProbeMap[entry.first] = probeIdx;
			mProbePositions[probeIdx] = entry.second.position;
			
			LightProbeInfo probeInfo;
			probeInfo.flags = LightProbeFlags::Dirty;
			probeInfo.bufferIdx = -1;
			probeInfo.handle = probeIdx;

			mProbeInfos[probeIdx] = probeInfo;
			probeIdx++;
		}
	}

	LightProbeVolume::~LightProbeVolume()
	{
		gRenderer()->notifyLightProbeVolumeRemoved(this);
	}

	void LightProbeVolume::initialize()
	{
		gRenderer()->notifyLightProbeVolumeAdded(this);

		CoreObject::initialize();
	}

	void LightProbeVolume::prune(Vector<UINT32>& freedEntries, bool freeAll)
	{
		UINT32 numProbes = (UINT32)mProbeInfos.size();
		INT32 lastSearchIdx = numProbes - 1;
		
		for (UINT32 i = 0; i < (UINT32)mProbeInfos.size(); ++i)
		{
			LightProbeInfo& info = mProbeInfos[i];

			if (info.flags == LightProbeFlags::Removed)
			{
				if (info.bufferIdx != -1)
					freedEntries.push_back(info.bufferIdx);

				info.flags = LightProbeFlags::Empty;

				// Replace the empty spot with an element from the back
				while (lastSearchIdx >= (INT32)i)
				{
					bool foundNonEmpty = false;
					LightProbeFlags flags = mProbeInfos[lastSearchIdx].flags;
					if (flags != LightProbeFlags::Empty)
					{
						std::swap(mProbeInfos[i], mProbeInfos[lastSearchIdx]);
						std::swap(mProbePositions[i], mProbePositions[lastSearchIdx]);

						mProbeMap[mProbeInfos[lastSearchIdx].handle] = i;
						foundNonEmpty = true;
					}

					// Remove last element
					mProbeInfos.erase(mProbeInfos.begin() + lastSearchIdx);
					mProbePositions.erase(mProbePositions.begin() + lastSearchIdx);
					lastSearchIdx--;

					// Search is done, we found an element to fill the empty spot
					if (foundNonEmpty)
						break;
				}
			}
		}

		if(freeAll)
		{
			// Add all remaining (non-removed) probes to the free list, and mark them as dirty so when/if those probes
			// get used again, the systems knows they are out of date
			for (UINT32 i = 0; i < (UINT32)mProbeInfos.size(); ++i)
			{
				LightProbeInfo& info = mProbeInfos[i];

				if (info.flags != LightProbeFlags::Empty)
				{
					if (info.bufferIdx != -1)
					{
						freedEntries.push_back(info.bufferIdx);
						info.bufferIdx = -1;
					}

					info.flags = LightProbeFlags::Dirty;
				}
			}
		}
	}

	void LightProbeVolume::syncToCore(const CoreSyncData& data)
	{
		char* dataPtr = (char*)data.getBuffer();

		bool oldIsActive = mIsActive;

		dataPtr = rttiReadElem(mPosition, dataPtr);
		dataPtr = rttiReadElem(mRotation, dataPtr);
		dataPtr = rttiReadElem(mIsActive, dataPtr);

		UINT32 numDirtyProbes, numRemovedProbes;
		dataPtr = rttiReadElem(numDirtyProbes, dataPtr);
		dataPtr = rttiReadElem(numRemovedProbes, dataPtr);

		for (UINT32 i = 0; i < numDirtyProbes; ++i)
		{
			UINT32 handle;
			dataPtr = rttiReadElem(handle, dataPtr);

			Vector3 position;
			dataPtr = rttiReadElem(position, dataPtr);

			LightProbeFlags flags;
			dataPtr = rttiReadElem(flags, dataPtr);

			auto iterFind = mProbeMap.find(handle);
			if(iterFind != mProbeMap.end())
			{
				UINT32 compactIdx = iterFind->second;
				
				mProbeInfos[compactIdx].flags = LightProbeFlags::Dirty;
				mProbePositions[compactIdx] = position;
			}
			else
			{
				UINT32 compactIdx = (UINT32)mProbeInfos.size();

				LightProbeInfo info;
				info.flags = LightProbeFlags::Dirty;
				info.bufferIdx = -1;
				info.handle = handle;

				mProbeInfos.push_back(info);
				mProbePositions.push_back(position);

				mProbeMap[handle] = compactIdx;
			}
		}

		for (UINT32 i = 0; i < numRemovedProbes; ++i)
		{
			UINT32 idx;
			dataPtr = rttiReadElem(idx, dataPtr);

			auto iterFind = mProbeMap.find(idx);
			if(iterFind != mProbeMap.end())
			{
				UINT32 compactIdx = iterFind->second;
				
				LightProbeInfo& info = mProbeInfos[compactIdx];
				info.flags = LightProbeFlags::Removed;
				
				mProbeMap.erase(iterFind);
			}
		}

		if (oldIsActive != mIsActive)
		{
			if (mIsActive)
				gRenderer()->notifyLightProbeVolumeAdded(this);
			else
				gRenderer()->notifyLightProbeVolumeRemoved(this);
		}
		else
		{
			if(mIsActive)
				gRenderer()->notifyLightProbeVolumeUpdated(this);
		}
	}
}}
