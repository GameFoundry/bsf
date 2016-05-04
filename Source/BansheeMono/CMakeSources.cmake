set(BS_BANSHEEMONO_INC_NOFILTER
	"Include/BsMonoPrerequisites.h"
	"Include/BsMonoAssembly.h"
	"Include/BsMonoClass.h"
	"Include/BsMonoField.h"
	"Include/BsMonoManager.h"
	"Include/BsMonoMethod.h"
	"Include/BsMonoProperty.h"
	"Include/BsMonoUtil.h"
	"Include/BsScriptMeta.h"
	"Include/BsMonoArray.h"
)

set(BS_BANSHEEMONO_SRC_NOFILTER
	"Source/BsMonoAssembly.cpp"
	"Source/BsMonoClass.cpp"
	"Source/BsMonoField.cpp"
	"Source/BsMonoManager.cpp"
	"Source/BsMonoMethod.cpp"
	"Source/BsMonoProperty.cpp"
	"Source/BsMonoPlugin.cpp"
	"Source/BsScriptMeta.cpp"
	"Source/BsMonoUtil.cpp"
	"Source/BsMonoArray.cpp"
)

source_group("Header Files" FILES ${BS_BANSHEEMONO_INC_NOFILTER})
source_group("Source Files" FILES ${BS_BANSHEEMONO_SRC_NOFILTER})

set(BS_BANSHEEMONO_SRC
	${BS_BANSHEEMONO_INC_NOFILTER}
	${BS_BANSHEEMONO_SRC_NOFILTER}
)