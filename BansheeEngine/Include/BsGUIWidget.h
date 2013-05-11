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

		/**
		 * @brief	Initializes the GUIWidget. Must be called in order for GUIWidget to start rendering.
		 *
		 * @param 	target			Target onto which we want to render the widget.
		 * @param	ownerWindow   	Window that contains the widget. This will be the source of all input
		 * 							for the widget. "target" and "ownerWindow" may be the same object.
		 */
		void initialize(CM::Viewport* target, const CM::RenderWindow* ownerWindow);

		void setSkin(const GUISkin* skin);
		const GUISkin* getGUISkin() const;

		bool inBounds(const CM::Int2& position) const;
		bool mouseEvent(const GUIMouseEvent& ev);

		virtual void render(CM::RenderContext& renderContext) const;

		const CM::RenderWindow* getOwnerWindow() const { return mOwnerWindow; }
	protected:
		friend class CM::SceneObject;
		friend class GUIElement;

		GUIWidget(const CM::HSceneObject& parent);

		void registerElement(GUIElement* elem);
	private:

		void updateMeshes() const;
		void updateBounds() const;

		const CM::RenderWindow* mOwnerWindow;
		std::vector<GUIElement*> mElements;
		
		mutable CM::ORect mBounds;
		mutable std::vector<std::pair<CM::ORect, GUIElement*>> mCachedBounds;
		mutable std::vector<CM::HMesh> mCachedMeshes;
		mutable std::vector<CM::HMaterial> mCachedMaterials;

		const GUISkin* mSkin;
		static GUISkin DefaultSkin;
	};
}