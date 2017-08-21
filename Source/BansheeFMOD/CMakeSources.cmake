set(BS_BANSHEEFMOD_INC_NOFILTER
	"BsFMODPrerequisites.h"
	"BsFMODAudio.h"
	"BsFMODImporter.h"
	"BsFMODAudioSource.h"
	"BsFMODAudioListener.h"
	"BsFMODAudioClip.h"
	"../BansheeOpenAudio/BsOggVorbisEncoder.h"
	"../BansheeOpenAudio/BsAudioDecoder.h"
	"../BansheeOpenAudio/BsOggVorbisDecoder.h"
)

set(BS_BANSHEEFMOD_SRC_NOFILTER
	"BsFMODPlugin.cpp"
	"BsFMODAudio.cpp"
	"BsFMODImporter.cpp"
	"BsFMODAudioSource.cpp"
	"BsFMODAudioListener.cpp"
	"BsFMODAudioClip.cpp"
	"../BansheeOpenAudio/BsOggVorbisEncoder.cpp"
	"../BansheeOpenAudio/BsOggVorbisDecoder.cpp"
)

source_group("Header Files" FILES ${BS_BANSHEEFMOD_INC_NOFILTER})
source_group("Source Files" FILES ${BS_BANSHEEFMOD_SRC_NOFILTER})

set(BS_BANSHEEFMOD_SRC
	${BS_BANSHEEFMOD_INC_NOFILTER}
	${BS_BANSHEEFMOD_SRC_NOFILTER}
)