set(BS_RENDERBEAST_INC_NOFILTER
	"Include/BsRenderTexturePool.h"
	"Include/BsRenderBeastOptions.h"
	"Include/BsSamplerOverrides.h"
	"Include/BsRenderBeast.h"
	"Include/BsRenderBeastFactory.h"
	"Include/BsRenderBeastPrerequisites.h"
	"Include/BsRenderTargets.h"
	"Include/BsStaticRenderableHandler.h"
	"Include/BsLightRendering.h"
	"Include/BsPostProcessing.h"
)

set(BS_RENDERBEAST_SRC_NOFILTER
	"Source/BsRenderTexturePool.cpp"
	"Source/BsSamplerOverrides.cpp"
	"Source/BsRenderBeast.cpp"
	"Source/BsRenderBeastFactory.cpp"
	"Source/BsRenderBeastPlugin.cpp"
	"Source/BsRenderTargets.cpp"
	"Source/BsStaticRenderableHandler.cpp"
	"Source/BsLightRendering.cpp"
	"Source/BsPostProcessing.cpp"
)

source_group("Header Files" FILES ${BS_RENDERBEAST_INC_NOFILTER})
source_group("Source Files" FILES ${BS_RENDERBEAST_SRC_NOFILTER})

set(BS_RENDERBEAST_SRC
	${BS_RENDERBEAST_INC_NOFILTER}
	${BS_RENDERBEAST_SRC_NOFILTER}
)