#pragma once

#include "CmPrerequisites.h"
#include "CmOverlay.h"
#include "CmTextSprite.h"

namespace CamelotEngine
{
	class CM_EXPORT GUIWidget : public Overlay
	{
	public:
		virtual ~GUIWidget();

		virtual void render(const Camera* camera, DeferredRenderContextPtr& renderContext) const;
	protected:
		friend class SceneObject;

		GUIWidget(const HSceneObject& parent);

		GUILabel* addLabel(const String& text, UINT32 fixedWidth = 0, UINT32 fixedHeight = 0, bool wordWrap = false, TextHorzAlign horzAlign = THA_Left, TextVertAlign vertAlign = TVA_Top);
		GUILabel* addLabel(const String& text, TextHorzAlign horzAlign = THA_Left, TextVertAlign vertAlign = TVA_Top);

		void setSkin(const GUISkin* skin);
		const GUISkin* getGUISkin() const;
	private:

		void updateMeshes() const;

		vector<GUIElement*>::type mElements;
		mutable vector<HMesh>::type mCachedMeshes;
		mutable vector<HMaterial>::type mCachedMaterials;
		const GUISkin* mSkin;
		static GUISkin DefaultSkin;
	};
}