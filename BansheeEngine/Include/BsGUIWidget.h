#pragma once

#include "BsPrerequisites.h"
#include "CmComponent.h"
#include "CmRectI.h"
#include "CmVector3.h"
#include "CmQuaternion.h"
#include <boost/signal.hpp>

namespace BansheeEngine
{
	class BS_EXPORT GUIWidget : public CM::Component
	{
	public:
		virtual ~GUIWidget();

		void setSkin(const GUISkin& skin);
		const GUISkin& getSkin() const;

		CM::UINT8 getDepth() const { return mDepth; }
		void setDepth(CM::UINT8 depth) { mDepth = depth; mWidgetIsDirty = true; }

		bool inBounds(const CM::Vector2I& position) const;
		const CM::RectI& getBounds() const { return mBounds; }

		/**
		 * @brief	Return true if widget or any of its elements are dirty.
		 *
		 * @param	cleanIfDirty	If true, all dirty elements will be updated and widget will be marked as clean.
		 *
		 * @return	True if dirty, false if not. If "cleanIfDirty" is true, the returned state is the one before cleaning.
		 */
		bool isDirty(bool cleanIfDirty);

		CM::Viewport* getTarget() const { return mTarget; }
		const CM::Vector<GUIElement*>::type& getElements() const { return mElements; }

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

		static GUISkin DefaultSkin;
	protected:
		friend class CM::SceneObject;
		friend class GUIElement;
		friend class GUIArea;
		friend class GUIManager;

		GUIWidget(const CM::HSceneObject& parent, CM::Viewport* target);

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

		CM::Viewport* mTarget;
		CM::Vector<GUIElement*>::type mElements;
		CM::Vector<GUIArea*>::type mAreas;
		CM::UINT8 mDepth;

		CM::Vector3 mLastFramePosition;
		CM::Quaternion mLastFrameRotation;
		CM::Vector3 mLastFrameScale;

		boost::signals::connection mOwnerTargetResizedConn;

		mutable bool mWidgetIsDirty;
		mutable CM::RectI mBounds;
		mutable CM::Vector<CM::HMesh>::type mCachedMeshes;
		mutable CM::Vector<CM::HMaterial>::type mCachedMaterials;

		const GUISkin* mSkin;
	};
}