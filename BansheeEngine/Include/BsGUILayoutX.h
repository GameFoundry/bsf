#pragma once

#include "BsPrerequisites.h"
#include "BsGUILayout.h"

namespace BansheeEngine
{
	class BS_EXPORT GUILayoutX : public GUILayout
	{
	public:
		GUILayoutX() {};
		~GUILayoutX() {};

		void _updateOptimalLayoutSizes();
	protected:
		void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height, RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth);
	};
}