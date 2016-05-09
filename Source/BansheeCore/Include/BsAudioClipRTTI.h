//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsAudioClip.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT AudioClipRTTI : public RTTIType <AudioClip, Resource, AudioClipRTTI>
	{
	private:
		BS_PLAIN_MEMBER_NAMED(readMode, mDesc.readMode);
		BS_PLAIN_MEMBER_NAMED(format, mDesc.format);
		BS_PLAIN_MEMBER_NAMED(frequency, mDesc.frequency);
		BS_PLAIN_MEMBER_NAMED(bitDepth, mDesc.bitDepth);
		BS_PLAIN_MEMBER_NAMED(numChannels, mDesc.numChannels);
		BS_PLAIN_MEMBER(mNumSamples);

		ManagedDataBlock getData(AudioClip* obj)
		{
			ManagedDataBlock dataBlock((UINT8*)obj->getData(), obj->getDataSize());
			return dataBlock;
		}

		void setData(AudioClip* obj, ManagedDataBlock val)
		{
			// TODO: This method isn't needed as this data will be loaded from the data stream
			obj->setData(val.getData(), obj->mNumSamples);
		}

		static UINT8* allocateData(AudioClip* obj, UINT32 numBytes)
		{
			// TODO: This method isn't needed as this data will be loaded from the data stream
			return (UINT8*)bs_alloc(numBytes);
		}

	public:
		AudioClipRTTI()
		{
			BS_ADD_PLAIN_FIELD(readMode, 0);
			BS_ADD_PLAIN_FIELD(format, 1);
			BS_ADD_PLAIN_FIELD(frequency, 2);
			BS_ADD_PLAIN_FIELD(bitDepth, 3);
			BS_ADD_PLAIN_FIELD(numChannels, 4);
			BS_ADD_PLAIN_FIELD(mNumSamples, 5);

			addDataBlockField("mData", 6, &AudioClipRTTI::getData, &AudioClipRTTI::setData, 
				0, &AudioClipRTTI::allocateData);
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