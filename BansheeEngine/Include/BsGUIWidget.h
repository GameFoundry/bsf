#pragma once

#include "BsPrerequisites.h"
#include "CmDeferredRenderContext.h"
#include "BsOverlay.h"
#include "BsTextSprite.h"
#include "CmORect.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIWidget : public Overlay
	{
	public:
		virtual ~GUIWidget();

		void setSkin(const GUISkin* skin);
		const GUISkin* getGUISkin() const;

		bool inBounds(const CM::Int2& position) const;
		bool mouseEvent(const GUIMouseEvent& ev);

		virtual void render(const Camera* camera, CM::RenderContext& renderContext) const;
	protected:
		friend class CM::SceneObject;
		friend class GUIElement;

		GUIWidget(const CM::HSceneObject& parent);

		void registerElement(GUIElement* elem);
	private:

		void updateMeshes() const;
		void updateBounds() const;

		std::vector<GUIElement*> mElements;
		
		mutable CM::ORect mBounds;
		mutable std::vector<std::pair<CM::ORect, GUIElement*>> mCachedBounds;
		mutable std::vector<CM::HMesh> mCachedMeshes;
		mutable std::vector<CM::HMaterial> mCachedMaterials;

		const GUISkin* mSkin;
		static GUISkin DefaultSkin;
	};
}