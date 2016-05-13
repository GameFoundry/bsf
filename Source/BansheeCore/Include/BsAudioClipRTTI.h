//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsAudioClip.h"
#include "BsDataStream.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT AudioClipRTTI : public RTTIType <AudioClip, Resource, AudioClipRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN_NAMED(readMode, mDesc.readMode, 0)
			BS_RTTI_MEMBER_PLAIN_NAMED(format, mDesc.format, 1)
			BS_RTTI_MEMBER_PLAIN_NAMED(frequency, mDesc.frequency, 2)
			BS_RTTI_MEMBER_PLAIN_NAMED(bitDepth, mDesc.bitDepth, 3)
			BS_RTTI_MEMBER_PLAIN_NAMED(numChannels, mDesc.numChannels, 4)
			BS_RTTI_MEMBER_PLAIN(mNumSamples, 5)
		BS_END_RTTI_MEMBERS

		SPtr<DataStream> getData(AudioClip* obj, UINT32& size)
		{
			size = obj->getDataSize();
			return bs_shared_ptr_new<MemoryDataStream>(obj->getData(), size, false);
		}

		void setData(AudioClip* obj, const SPtr<DataStream>& val, UINT32 size)
		{
			// TODO - Load from steam
		}

	public:
		AudioClipRTTI()
			:mInitMembers(this)
		{
			addDataBlockField("mData", 6, &AudioClipRTTI::getData, &AudioClipRTTI::setData, 0);
		}

		const String& getRTTIName() override
		{
			static String name = "AudioClip";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_AudioClip;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			AUDIO_CLIP_DESC desc;
			return AudioClip::_createPtr(nullptr, 0, desc); // Initial values don't matter, they will get overwritten
		}
	};

	/** @} */
	/** @endcond */
}