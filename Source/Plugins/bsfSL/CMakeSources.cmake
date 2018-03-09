set(BS_SL_INC_NOFILTER
	"BsSLPrerequisites.h"
	"BsASTFX.h"
	"BsMMAlloc.h"
	"BsSLImporter.h"
	"BsSLFXCompiler.h"
	"BsIncludeHandler.h"
	"BsLexerFX.h"
	"BsParserFX.h"
)

set(BS_SL_SRC_NOFILTER
	"BsSLPlugin.cpp"
	"BsASTFX.c"
	"BsSLImporter.cpp"
	"BsSLFXCompiler.cpp"
	"BsIncludeHandler.cpp"
	"BSMMAlloc.c"
	"BsLexerFX.c"
	"BsParserFX.c"
)

source_group("" FILES ${BS_SL_INC_NOFILTER} ${BS_SL_SRC_NOFILTER})

set(BS_SL_SRC
	${BS_SL_INC_NOFILTER}
	${BS_SL_SRC_NOFILTER}
)
