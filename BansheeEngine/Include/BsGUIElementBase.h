#pragma once

#include "BsPrerequisites.h"
#include "BsGUILayoutOptions.h"
#include "CmRect.h"
#include "CmInt2.h"

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

		/************************************************************************/
		/* 							INTERNAL METHODS                      		*/
		/************************************************************************/

		/**
		 * @brief	Re-arranges the elements to fit the layout. (Internal use only)
		 */
		virtual void _updateLayout(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height, 
			CM::Rect clipRect, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);
		virtual void _updateOptimalLayoutSizes();
		virtual void _updateLayoutInternal(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height,
			CM::Rect clipRect, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);

		void _setParent(GUIElementBase* parent) { mParentElement = parent; }

		virtual CM::UINT32 _getOptimalWidth() const = 0;
		virtual CM::UINT32 _getOptimalHeight() const = 0;
		virtual Type _getType() const = 0;
		GUIElementBase* _getParent() const { return mParentElement; }

		void _markAsClean() { mIsDirty = 0; }
		virtual void _setFocus(bool focus) {}

		bool _isContentDirty() const;
		bool _isMeshDirty() const; 

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
		GUILayout& insertLayoutXInternal(GUIElementBase* parent, CM::UINT32 idx);
		GUILayout& insertLayoutYInternal(GUIElementBase* parent, CM::UINT32 idx);

		GUIElementBase* mParentElement;
		CM::Vector<GUIElementBase*>::type mChildren;	
		CM::UINT8 mIsDirty;
	};
}