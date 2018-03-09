set(BS_OPENAUDIO_INC_NOFILTER
	"BsOAPrerequisites.h"
	"BsOAImporter.h"
	"BsWaveDecoder.h"
	"BsOggVorbisDecoder.h"
	"BsFLACDecoder.h"
	"BsAudioDecoder.h"
	"BsOggVorbisEncoder.h"
	"BsOAAudioClip.h"
	"BsOAAudio.h"
	"BsOAAudioSource.h"
	"BsOAAudioListener.h"
)

set(BS_OPENAUDIO_SRC_NOFILTER
	"BsOAPlugin.cpp"
	"BsOAImporter.cpp"
	"BsWaveDecoder.cpp"
	"BsOggVorbisDecoder.cpp"
	"BsFLACDecoder.cpp"
	"BsOggVorbisEncoder.cpp"
	"BsOAAudioClip.cpp"
	"BsOAAudio.cpp"
	"BsOAAudioSource.cpp"
	"BsOAAudioListener.cpp"
)

source_group("" FILES ${BS_OPENAUDIO_INC_NOFILTER} ${BS_OPENAUDIO_SRC_NOFILTER})

set(BS_OPENAUDIO_SRC
	${BS_OPENAUDIO_INC_NOFILTER}
	${BS_OPENAUDIO_SRC_NOFILTER}
)