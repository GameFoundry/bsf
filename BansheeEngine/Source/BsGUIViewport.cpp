#include "BsGUIViewport.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsGUILayoutOptions.h"
#include "BsCamera.h"
#include "CmViewport.h"
#include "CmRenderTarget.h"
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String& GUIViewport::getGUITypeName()
	{
		static String name = "Viewport";
		return name;
	}

	GUIViewport::GUIViewport(GUIWidget& parent, const GUIElementStyle* style, const HCamera& camera, 
		float aspectRatio, CM::Degree fieldOfView, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions), mCamera(camera), mAspectRatio(aspectRatio),
		mFieldOfView(fieldOfView)
	{
		RenderTargetPtr guiRenderTarget = parent.getTarget()->getTarget();
		RenderTargetPtr cameraRenderTarget = camera->getViewport()->getTarget();

		if(guiRenderTarget != cameraRenderTarget)
			CM_EXCEPT(InvalidParametersException, "Camera provided to GUIViewport must use the same render target as the GUIWidget this element is located on.")

		mVerticalFOV = 2.0f * Math::ATan(Math::Tan(mFieldOfView.valueRadians() * 0.5f) * (1.0f / mAspectRatio));
	}

	GUIViewport::~GUIViewport()
	{

	}

	GUIViewport* GUIViewport::create(GUIWidget& parent, const HCamera& camera, float aspectRatio, CM::Degree fieldOfView, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIViewport, PoolAlloc>()) GUIViewport(parent, style, camera, aspectRatio, fieldOfView, getDefaultLayoutOptions(style));
	}

	GUIViewport* GUIViewport::create(GUIWidget& parent, const GUILayoutOptions& layoutOptions, const HCamera& camera, 
		float aspectRatio, CM::Degree fieldOfView, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIViewport, PoolAlloc>()) GUIViewport(parent, style, camera, aspectRatio, fieldOfView, layoutOptions);
	}

	UINT32 GUIViewport::getNumRenderElements() const
	{
		return 0;
	}

	const HMaterial& GUIViewport::getMaterial(UINT32 renderElementIdx) const
	{
		CM_EXCEPT(InternalErrorException, "This element has no render element so no material can be retrieved.");
	}

	UINT32 GUIViewport::getNumQuads(UINT32 renderElementIdx) const
	{
		return 0;
	}

	void GUIViewport::updateClippedBounds()
	{
		Rect mBounds = Rect(0, 0, mWidth, mHeight);
		mBounds.clip(mClipRect);
		mBounds.x += mOffset.x;
		mBounds.y += mOffset.y;
	}

	UINT32 GUIViewport::_getOptimalWidth() const
	{
		return 0;
	}

	UINT32 GUIViewport::_getOptimalHeight() const
	{
		return 0;
	}

	void GUIViewport::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{

	}

	void GUIViewport::updateRenderElementsInternal()
	{
		// TODO - This doesn't get called if element mesh is dirty!!! and I need to update the viewport when offset changes (in which case mesh is marked as dirty)
		float currentAspect = mWidth / (float)mHeight;
		Radian currentFOV = 2.0f * Math::ATan(Math::Tan(mVerticalFOV * 0.5f) * currentAspect);

		mCamera->setHorzFOV(currentFOV);

		ViewportPtr viewport = mCamera->getViewport();
		RenderTargetPtr renderTarget = viewport->getTarget();

		float x = mOffset.x / (float)renderTarget->getWidth();
		float y = mOffset.y / (float)renderTarget->getHeight();
		float width = mWidth / (float)renderTarget->getWidth();
		float height = mHeight / (float)renderTarget->getHeight();

		viewport->setDimensions(x, y, width, height);
	}
}