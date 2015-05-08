#pragma once

#include "BsPrerequisites.h"
#include "BsComponent.h"
#include "BsRect2I.h"
#include "BsVector3.h"
#include "BsQuaternion.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	A top level container for all types of GUI elements. Every GUI element, layout or area
	 *			must be assigned to a widget in order to be rendered.
	 *
	 *			Widgets are the only GUI objects that may be arbitrarily transformed, allowing you to create
	 *			3D interfaces.
	 */
	class BS_EXPORT GUIWidget : public Component
	{
	public:
		virtual ~GUIWidget();

		/**
		 * @brief	Sets the skin used for all GUI elements in the widget. This will update
		 *			the look of all current elements.
		 */
		void setSkin(const HGUISkin& skin);

		/**
		 * @brief	Returns the currently active GUI skin.
		 */
		const GUISkin& getSkin() const;

		/**
		 * @brief	Returns the currently active GUI skin resource.
		 */
		const HGUISkin& getSkinResource() const { return mSkin; }

		/**
		 * @brief	Returns the root GUI panel for the widget.
		 */
		GUIPanel* getPanel() const { return mPanel; }

		/**
		 * @brief	Returns the depth to render the widget at. If two widgets overlap the
		 *			widget with the lower depth will be rendered in front.
		 */
		UINT8 getDepth() const { return mDepth; }

		/**
		 * @brief	Changes the depth to render the widget at. If two widgets overlap the
		 *			widget with the lower depth will be rendered in front.
		 */
		void setDepth(UINT8 depth) { mDepth = depth; mWidgetIsDirty = true; }

		/**
		 * @brief	Checks are the specified coordinates within widget bounds. Coordinates should
		 *			be relative to the parent window.
		 */
		bool inBounds(const Vector2I& position) const;

		/**
		 * @brief	Returns bounds of the widget, relative to the parent window.
		 */
		const Rect2I& getBounds() const { return mBounds; }

		/**
		 * @brief	Return true if widget or any of its elements are dirty.
		 *
		 * @param	cleanIfDirty	If true, all dirty elements will be updated and widget will be marked as clean.
		 *
		 * @return	True if dirty, false if not. If "cleanIfDirty" is true, the returned state is the one before cleaning.
		 */
		bool isDirty(bool cleanIfDirty);

		/**
		 * @brief	Returns the viewport that this widget will be rendered on.
		 */
		Viewport* getTarget() const { return mTarget; }

		/**
		 * @brief	Returns a list of all elements parented to this widget.
		 */
		const Vector<GUIElement*>& getElements() const { return mElements; }

		/**
		 * @brief	Registers a new element as a child of the widget.
		 *
		 * @note	Internal method.
		 */
		void _registerElement(GUIElementBase* elem);
		
		/**
		 * @brief	Unregisters an element from the widget. Usually called when the element
		 *			is destroyed, or reparented to another widget.
		 *
		 * @note	Internal method.
		 */
		void _unregisterElement(GUIElementBase* elem);

		/**
		 * @brief	Marks the widget mesh dirty requiring a mesh rebuild. Provided element
		 *			is the one that requested the mesh update.
		 */
		void _markMeshDirty(GUIElementBase* elem);

		/**
		 * @brief	Updates the layout of all child elements, repositioning and resizing them as needed.
		 */
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

	protected:
		friend class SceneObject;
		friend class GUIElementBase;
		friend class GUIManager;

		/**
		 * @brief	Constructs a new GUI widget attached to the specified parent scene object.
		 *			Widget elements will be rendered on the provided viewport.
		 */
		GUIWidget(const HSceneObject& parent, Viewport* target);

		/**
		 * @brief	Called when the viewport size changes and widget elements need to be updated.
		 */
		virtual void ownerTargetResized();

		/**
		 * @brief	Called when the parent window gained or lost focus.
		 */
		virtual void ownerWindowFocusChanged();

		/**
		 * @copydoc	Component::update
		 */
		virtual void update();

		/**
		 * @copydoc	Component::onDestroyed
		 */
		virtual void onDestroyed();
	private:
		GUIWidget(const GUIWidget& other) { }

		/**
		 * @brief	Calculates widget bounds using the bounds of all child elements.
		 */
		void updateBounds() const;

		/**
		 * @brief	Updates the size of the primary GUI panel based on the viewport.
		 */
		void updatePanelSize();

		Viewport* mTarget;
		Vector<GUIElement*> mElements;
		GUIPanel* mPanel;
		UINT8 mDepth;

		Vector3 mLastFramePosition;
		Quaternion mLastFrameRotation;
		Vector3 mLastFrameScale;

		HEvent mOwnerTargetResizedConn;

		Vector<GUIElement*> mDirtyContents;

		mutable bool mWidgetIsDirty;
		mutable Rect2I mBounds;
		mutable Vector<HMesh> mCachedMeshes;
		mutable Vector<HMaterial> mCachedMaterials;

		HGUISkin mSkin;
	};
}