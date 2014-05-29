#pragma once

#include "BsPrerequisites.h"
#include "BsComponent.h"
#include "BsRectI.h"
#include "BsVector3.h"
#include "BsQuaternion.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIWidget : public Component
	{
	public:
		virtual ~GUIWidget();

		void setSkin(const GUISkin& skin);
		const GUISkin& getSkin() const;

		UINT8 getDepth() const { return mDepth; }
		void setDepth(UINT8 depth) { mDepth = depth; mWidgetIsDirty = true; }

		bool inBounds(const Vector2I& position) const;
		const RectI& getBounds() const { return mBounds; }

		/**
		 * @brief	Return true if widget or any of its elements are dirty.
		 *
		 * @param	cleanIfDirty	If true, all dirty elements will be updated and widget will be marked as clean.
		 *
		 * @return	True if dirty, false if not. If "cleanIfDirty" is true, the returned state is the one before cleaning.
		 */
		bool isDirty(bool cleanIfDirty);

		Viewport* getTarget() const { return mTarget; }
		const Vector<GUIElement*>& getElements() const { return mElements; }

		void _updateLayout();

		/**
		 * @brief	Forwards the specified mouse event to the specified element. The element
		 * 			must be a child of this widget.
		 */
		virtual bool _mouseEvent(GUIElement* element, const GUIMouseEvent& ev);
				
		/**
		 * @brief	Forwards the specified key event to the specified element. The element
		 * 			must be a child of this widget.
		 */
		virtual bool _textInputEvent(GUIElement* element, const GUITextInputEvent& ev);

		/**
		 * @brief	Forwards the specified key event to the specified element. The element
		 * 			must be a child of this widget.
		 */
		virtual bool _commandEvent(GUIElement* element, const GUICommandEvent& ev);

		/**
		 * @brief	Forwards the specified virtual button event to the specified element. The element
		 * 			must be a child of this widget.
		 */
		virtual bool _virtualButtonEvent(GUIElement* element, const GUIVirtualButtonEvent& ev);

		static GUISkin DefaultSkin;
	protected:
		friend class SceneObject;
		friend class GUIElement;
		friend class GUIArea;
		friend class GUIManager;

		GUIWidget(const HSceneObject& parent, Viewport* target);

		void registerElement(GUIElement* elem);
		void unregisterElement(GUIElement* elem);

		void registerArea(GUIArea* area);
		void unregisterArea(GUIArea* area);

		virtual void ownerTargetResized();
		virtual void ownerWindowFocusChanged();

		virtual void update();
	private:
		GUIWidget(const GUIWidget& other) { }

		void updateBounds() const;

		Viewport* mTarget;
		Vector<GUIElement*> mElements;
		Vector<GUIArea*> mAreas;
		UINT8 mDepth;

		Vector3 mLastFramePosition;
		Quaternion mLastFrameRotation;
		Vector3 mLastFrameScale;

		HEvent mOwnerTargetResizedConn;

		mutable bool mWidgetIsDirty;
		mutable RectI mBounds;
		mutable Vector<HMesh> mCachedMeshes;
		mutable Vector<HMaterial> mCachedMaterials;

		const GUISkin* mSkin;
	};
}