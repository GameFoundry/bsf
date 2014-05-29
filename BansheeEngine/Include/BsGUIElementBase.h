#pragma once

#include "BsPrerequisites.h"
#include "BsGUIMaterialInfo.h"
#include "BsGUILayoutOptions.h"
#include "BsRectI.h"
#include "BsVector2I.h"
#include "BsRectOffset.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIElementBase
	{
	public:
		enum class Type
		{
			Layout,
			Element,
			FixedSpace,
			FlexibleSpace
		};

	public:
		GUIElementBase();
		virtual ~GUIElementBase();

		/**
		 * @brief	Enables (default) this element and all its children.
		 */
		void enableRecursively();

		/**
		 * @brief	Disables this element and all its children.
		 */
		void disableRecursively();

		/************************************************************************/
		/* 							INTERNAL METHODS                      		*/
		/************************************************************************/

		/**
		 * @brief	Re-arranges the elements to fit the layout. (Internal use only)
		 */
		virtual void _updateLayout(INT32 x, INT32 y, UINT32 width, UINT32 height, 
			RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth);
		virtual void _updateOptimalLayoutSizes();
		virtual void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth);

		void _setParent(GUIElementBase* parent);

		virtual Vector2I _getOptimalSize() const = 0;
		virtual const RectOffset& _getPadding() const = 0;
		virtual Type _getType() const = 0;
		GUIElementBase* _getParent() const { return mParentElement; }
		GUIWidget* _getParentWidget() const { return mParentWidget; }

		void _markAsClean() { mIsDirty = 0; }

		bool _isContentDirty() const;
		bool _isMeshDirty() const; 
		bool _isDisabled() const { return mIsDisabled; }

		virtual void _changeParentWidget(GUIWidget* widget);

		void _registerChildElement(GUIElement* element);
		void _unregisterChildElement(GUIElement* element);

	protected:
		/**
		 * @brief	Marks the elements contents as dirty, which causes the sprite meshes to be recreated from scratch.
		 */
		void markContentAsDirty();

		/**
		 * @brief	Mark only the elements that operate directly on the sprite mesh without requiring the mesh
		 * 			to be recreated as dirty. This includes position, depth and clip rectangle.
		 */
		void markMeshAsDirty();

		GUILayout& addLayoutXInternal(GUIElementBase* parent);
		GUILayout& addLayoutYInternal(GUIElementBase* parent);
		void removeLayoutInternal(GUILayout& layout);
		GUILayout& insertLayoutXInternal(GUIElementBase* parent, UINT32 idx);
		GUILayout& insertLayoutYInternal(GUIElementBase* parent, UINT32 idx);

		GUIWidget* mParentWidget;
		GUIElementBase* mParentElement;
		Vector<GUIElementBase*> mChildren;	
		UINT8 mIsDirty;
		bool mIsDisabled;
	};
}