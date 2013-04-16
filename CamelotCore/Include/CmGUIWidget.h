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

		void setSkin(const GUISkin* skin);
		const GUISkin* getGUISkin() const;

		virtual void render(const Camera* camera, DeferredRenderContextPtr& renderContext) const;
	protected:
		friend class SceneObject;
		friend class GUIElement;

		GUIWidget(const HSceneObject& parent);

		void mouseEvent(const GUIMouseEvent& ev);

		void registerElement(GUIElement* elem);
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