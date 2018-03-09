set(BS_FMOD_INC_NOFILTER
	"BsFMODPrerequisites.h"
	"BsFMODAudio.h"
	"BsFMODImporter.h"
	"BsFMODAudioSource.h"
	"BsFMODAudioListener.h"
	"BsFMODAudioClip.h"
	"../bsfOpenAudio/BsOggVorbisEncoder.h"
	"../bsfOpenAudio/BsAudioDecoder.h"
	"../bsfOpenAudio/BsOggVorbisDecoder.h"
)

set(BS_FMOD_SRC_NOFILTER
	"BsFMODPlugin.cpp"
	"BsFMODAudio.cpp"
	"BsFMODImporter.cpp"
	"BsFMODAudioSource.cpp"
	"BsFMODAudioListener.cpp"
	"BsFMODAudioClip.cpp"
	"../bsfOpenAudio/BsOggVorbisEncoder.cpp"
	"../bsfOpenAudio/BsOggVorbisDecoder.cpp"
)

source_group("" FILES ${BS_FMOD_INC_NOFILTER} ${BS_FMOD_SRC_NOFILTER})

set(BS_FMOD_SRC
	${BS_FMOD_INC_NOFILTER}
	${BS_FMOD_SRC_NOFILTER}
)