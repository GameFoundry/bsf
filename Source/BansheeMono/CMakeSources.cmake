set(BS_BANSHEEMONO_INC_NOFILTER
	"BsMonoPrerequisites.h"
	"BsMonoAssembly.h"
	"BsMonoClass.h"
	"BsMonoField.h"
	"BsMonoManager.h"
	"BsMonoMethod.h"
	"BsMonoProperty.h"
	"BsMonoUtil.h"
	"BsScriptMeta.h"
	"BsMonoArray.h"
)

set(BS_BANSHEEMONO_SRC_NOFILTER
	"BsMonoAssembly.cpp"
	"BsMonoClass.cpp"
	"BsMonoField.cpp"
	"BsMonoManager.cpp"
	"BsMonoMethod.cpp"
	"BsMonoProperty.cpp"
	"BsMonoPlugin.cpp"
	"BsScriptMeta.cpp"
	"BsMonoUtil.cpp"
	"BsMonoArray.cpp"
)

source_group("Header Files" FILES ${BS_BANSHEEMONO_INC_NOFILTER})
source_group("Source Files" FILES ${BS_BANSHEEMONO_SRC_NOFILTER})

set(BS_BANSHEEMONO_SRC
	${BS_BANSHEEMONO_INC_NOFILTER}
	${BS_BANSHEEMONO_SRC_NOFILTER}
)