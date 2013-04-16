#pragma once

#include "BsPrerequisites.h"
#include "CmOverlay.h"
#include "BsTextSprite.h"
#include "CmORect.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIWidget : public CM::Overlay
	{
	public:
		virtual ~GUIWidget();

		void setSkin(const GUISkin* skin);
		const GUISkin* getGUISkin() const;

		virtual void render(const CM::Camera* camera, CM::DeferredRenderContextPtr& renderContext) const;
	protected:
		friend class SceneObject;
		friend class GUIElement;

		GUIWidget(const CM::HSceneObject& parent);

		void mouseEvent(const GUIMouseEvent& ev);

		void registerElement(GUIElement* elem);
	private:

		void updateMeshes() const;
		void updateBounds() const;

		std::vector<GUIElement*> mElements;
		
		mutable CM::Rect mBounds;
		mutable std::vector<std::pair<CM::ORect, GUIElement*>> mCachedBounds;
		mutable std::vector<CM::HMesh> mCachedMeshes;
		mutable std::vector<CM::HMaterial> mCachedMaterials;

		const GUISkin* mSkin;
		static GUISkin DefaultSkin;
	};
}