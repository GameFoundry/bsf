set(BS_BANSHEESL_INC_NOFILTER
	"Include/BsSLPrerequisites.h"
	"Include/BsASTFX.h"
	"Include/BsMMAlloc.h"
	"Include/BsSLImporter.h"
	"Include/BsSLFXCompiler.h"
	"Include/BsIncludeHandler.h"
	"Include/BsLexerFX.h"
	"Include/BsParserFX.h"
)

set(BS_BANSHEESL_SRC_NOFILTER
	"Source/BsSLPlugin.cpp"
	"Source/BsASTFX.c"
	"Source/BsSLImporter.cpp"
	"Source/BsSLFXCompiler.cpp"
	"Source/BsIncludeHandler.cpp"
	"Source/BSMMAlloc.c"
	"Source/BsLexerFX.c"
	"Source/BsParserFX.c"
)

source_group("Header Files" FILES ${BS_BANSHEESL_INC_NOFILTER})
source_group("Source Files" FILES ${BS_BANSHEESL_SRC_NOFILTER})

set(BS_BANSHEESL_SRC
	${BS_BANSHEESL_INC_NOFILTER}
	${BS_BANSHEESL_SRC_NOFILTER}
)