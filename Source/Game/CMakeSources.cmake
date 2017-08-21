set(BS_GAME_INC_NOFILTER
	"resource.h"
)

set(BS_GAME_SRC_NOFILTER
	"Main.cpp"
)

source_group("Header Files" FILES ${BS_GAME_INC_NOFILTER})
source_group("Source Files" FILES ${BS_GAME_SRC_NOFILTER})

set(BS_GAME_SRC
	${BS_GAME_INC_NOFILTER}
	${BS_GAME_SRC_NOFILTER}
)