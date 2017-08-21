set(BS_RENDERBEAST_INC_NOFILTER
	"BsGpuResourcePool.h"
	"BsRenderBeastOptions.h"
	"BsSamplerOverrides.h"
	"BsRenderBeast.h"
	"BsRenderBeastFactory.h"
	"BsRenderBeastPrerequisites.h"
	"BsObjectRendering.h"
	"BsLightRendering.h"
	"BsPostProcessing.h"
	"BsRendererView.h"
	"BsRendererObject.h"
	"BsLightGrid.h"
	"BsImageBasedLighting.h"
	"BsShadowRendering.h"
	"BsRendererScene.h"
	"BsStandardDeferredLighting.h"
	"BsLightProbes.h"
	"BsRenderCompositor.h"
	"BsRendererTextures.h"
	"BsRenderBeastIBLUtility.h"
)

set(BS_RENDERBEAST_SRC_NOFILTER
	"BsGpuResourcePool.cpp"
	"BsSamplerOverrides.cpp"
	"BsRenderBeast.cpp"
	"BsRenderBeastFactory.cpp"
	"BsRenderBeastPlugin.cpp"
	"BsObjectRendering.cpp"
	"BsLightRendering.cpp"
	"BsPostProcessing.cpp"
	"BsRendererView.cpp"
	"BsRendererObject.cpp"
	"BsLightGrid.cpp"
	"BsImageBasedLighting.cpp"
	"BsShadowRendering.cpp"
	"BsRendererScene.cpp"
	"BsStandardDeferredLighting.cpp"
	"BsLightProbes.cpp"
	"BsRenderCompositor.cpp"
	"BsRendererTextures.cpp"
	"BsRenderBeastIBLUtility.cpp"
)

source_group("Header Files" FILES ${BS_RENDERBEAST_INC_NOFILTER})
source_group("Source Files" FILES ${BS_RENDERBEAST_SRC_NOFILTER})

set(BS_RENDERBEAST_SRC
	${BS_RENDERBEAST_INC_NOFILTER}
	${BS_RENDERBEAST_SRC_NOFILTER}
)