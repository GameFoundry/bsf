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

		virtual void render(CM::RenderContext& renderContext) const;

		const CM::RenderWindow* getOwnerWindow() const { return mOwnerWindow; }
		const std::vector<GUIElement*>& getElements() const { return mElements; }
	protected:
		friend class CM::SceneObject;
		friend class GUIElement;
		friend class GUIArea;

		GUIWidget(const CM::HSceneObject& parent);

		void registerElement(GUIElement* elem);
		void unregisterElement(GUIElement* elem);

		void registerArea(GUIArea* area);
		void unregisterArea(GUIArea* area);
	private:
		void updateMeshes() const;
		void updateBounds() const;

		const CM::RenderWindow* mOwnerWindow;
		std::vector<GUIElement*> mElements;
		std::vector<GUIArea*> mAreas;

		mutable bool mWidgetIsDirty;
		mutable CM::Rect mBounds;
		mutable std::vector<CM::HMesh> mCachedMeshes;
		mutable std::vector<CM::HMaterial> mCachedMaterials;

		const GUISkin* mSkin;
		static GUISkin DefaultSkin;
	};
}