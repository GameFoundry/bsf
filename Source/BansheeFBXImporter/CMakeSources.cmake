set(BS_BANSHEEFBXIMPORTER_INC_NOFILTER
	"Include/BsFBXPrerequisites.h"
	"Include/BsFBXImporter.h"
	"Include/BsFBXImportData.h"
	"Include/BsFBXUtility.h"
)

set(BS_BANSHEEFBXIMPORTER_SRC_NOFILTER
	"Source/BsFBXPlugin.cpp"
	"Source/BsFBXImporter.cpp"
	"Source/BsFBXUtility.cpp"
	"Source/BsFBXImportData.cpp"
)

source_group("Header Files" FILES ${BS_BANSHEEFBXIMPORTER_INC_NOFILTER})
source_group("Source Files" FILES ${BS_BANSHEEFBXIMPORTER_SRC_NOFILTER})

set(BS_BANSHEEFBXIMPORTER_SRC
	${BS_BANSHEEFBXIMPORTER_INC_NOFILTER}
	${BS_BANSHEEFBXIMPORTER_SRC_NOFILTER}
)