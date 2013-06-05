#pragma once

#include "BsPrerequisites.h"
#include "CmComponent.h"
#include "CmRect.h"
#include "CmVector3.h"
#include "CmQuaternion.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIWidget : public CM::Component
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
		virtual void initialize(CM::Viewport* target, const CM::RenderWindow* ownerWindow);

		void setSkin(const GUISkin* skin);
		const GUISkin* getSkin() const;

		CM::UINT8 getDepth() const { return mDepth; }
		void setDepth(CM::UINT8 depth) { mDepth = depth; mWidgetIsDirty = true; }

		bool inBounds(const CM::Int2& position) const;

		/**
		 * @brief	Return true if widget or any of its elements are dirty.
		 *
		 * @param	cleanIfDirty	If true, all dirty elements will be updated and widget will be marked as clean.
		 *
		 * @return	True if dirty, false if not. If "cleanIfDirty" is true, the returned state is the one before cleaning.
		 */
		bool isDirty(bool cleanIfDirty);

		const CM::RenderWindow* getOwnerWindow() const { return mOwnerWindow; }
		CM::Viewport* getTarget() const { return mTarget; }
		const CM::vector<GUIElement*>::type& getElements() const { return mElements; }

		void _updateLayout();

		/**
		 * @brief	Forwards the specified mouse event to the specified element. The element
		 * 			must be a child of this widget.
		 */
		virtual bool _mouseEvent(GUIElement* element, const GUIMouseEvent& ev);
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
		void updateBounds() const;

		void ownerWindowResized(CM::RenderWindow* window);

		virtual void update();

		const CM::RenderWindow* mOwnerWindow;
		CM::Viewport* mTarget;
		CM::vector<GUIElement*>::type mElements;
		CM::vector<GUIArea*>::type mAreas;
		CM::UINT8 mDepth;

		CM::Vector3 mLastFramePosition;
		CM::Quaternion mLastFrameRotation;
		CM::Vector3 mLastFrameScale;

		mutable bool mWidgetIsDirty;
		mutable CM::Rect mBounds;
		mutable CM::vector<CM::HMesh>::type mCachedMeshes;
		mutable CM::vector<CM::HMaterial>::type mCachedMaterials;

		const GUISkin* mSkin;
		static GUISkin DefaultSkin;
	};
}