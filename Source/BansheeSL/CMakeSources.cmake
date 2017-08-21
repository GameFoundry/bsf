set(BS_BANSHEESL_INC_NOFILTER
	"BsSLPrerequisites.h"
	"BsASTFX.h"
	"BsMMAlloc.h"
	"BsSLImporter.h"
	"BsSLFXCompiler.h"
	"BsIncludeHandler.h"
	"BsLexerFX.h"
	"BsParserFX.h"
)

set(BS_BANSHEESL_SRC_NOFILTER
	"BsSLPlugin.cpp"
	"BsASTFX.c"
	"BsSLImporter.cpp"
	"BsSLFXCompiler.cpp"
	"BsIncludeHandler.cpp"
	"BSMMAlloc.c"
	"BsLexerFX.c"
	"BsParserFX.c"
)

source_group("Header Files" FILES ${BS_BANSHEESL_INC_NOFILTER})
source_group("Source Files" FILES ${BS_BANSHEESL_SRC_NOFILTER})

set(BS_BANSHEESL_SRC
	${BS_BANSHEESL_INC_NOFILTER}
	${BS_BANSHEESL_SRC_NOFILTER}
)
