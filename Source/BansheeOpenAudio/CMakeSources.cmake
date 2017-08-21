set(BS_BANSHEEOPENAUDIO_INC_NOFILTER
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

set(BS_BANSHEEOPENAUDIO_SRC_NOFILTER
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

source_group("Header Files" FILES ${BS_BANSHEEOPENAUDIO_INC_NOFILTER})
source_group("Source Files" FILES ${BS_BANSHEEOPENAUDIO_SRC_NOFILTER})

set(BS_BANSHEEOPENAUDIO_SRC
	${BS_BANSHEEOPENAUDIO_INC_NOFILTER}
	${BS_BANSHEEOPENAUDIO_SRC_NOFILTER}
)