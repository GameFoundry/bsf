//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsImportOptions.h"
#include "BsAudioClip.h"

namespace BansheeEngine
{
	/** @addtogroup Importer
	 *  @{
	 */

	/** Contains import options you may use to control how an audio clip is imported. */
	class BS_CORE_EXPORT AudioClipImportOptions : public ImportOptions
	{
	public:
		AudioFormat getFormat() const { return mFormat; }
		void setFormat(AudioFormat format) { mFormat = format; }

		AudioReadMode getReadMode() const { return mReadMode; }
		void setReadMode(AudioReadMode readMode) { mReadMode = readMode; }

		bool getIs3D() const { return mIs3D; }
		void setIs3D(bool is3d) { mIs3D = is3d; }

		// Note: Add options to resample to a different frequency, reduce/increase bit depth

	private:
		AudioFormat mFormat = AudioFormat::PCM;
		AudioReadMode mReadMode = AudioReadMode::LoadDecompressed;
		bool mIs3D = true;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class AudioClipImportOptionsRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** @} */
}