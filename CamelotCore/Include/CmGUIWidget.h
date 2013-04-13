#pragma once

#include "CmPrerequisites.h"
#include "CmOverlay.h"
#include "CmTextSprite.h"
#include "CmORect.h"

namespace CamelotEngine
{
	class CM_EXPORT GUIWidget : public Overlay
	{
	public:
		virtual ~GUIWidget();

		GUILabel* addLabel(const String& text, UINT32 fixedWidth = 0, UINT32 fixedHeight = 0, bool wordWrap = false, TextHorzAlign horzAlign = THA_Left, TextVertAlign vertAlign = TVA_Top);
		GUILabel* addLabel(const String& text, TextHorzAlign horzAlign = THA_Left, TextVertAlign vertAlign = TVA_Top);

		void setSkin(const GUISkin* skin);
		const GUISkin* getGUISkin() const;

		virtual void render(const Camera* camera, DeferredRenderContextPtr& renderContext) const;
	protected:
		friend class SceneObject;

		GUIWidget(const HSceneObject& parent);

		void mouseEvent(const GUIMouseEvent& ev);
	private:

		void updateMeshes() const;
		void updateBounds() const;

		vector<GUIElement*>::type mElements;
		
		mutable Rect mBounds;
		mutable vector<std::pair<ORect, GUIElement*>>::type mCachedBounds;
		mutable vector<HMesh>::type mCachedMeshes;
		mutable vector<HMaterial>::type mCachedMaterials;

		const GUISkin* mSkin;
		static GUISkin DefaultSkin;
	};
}