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

		void _updateOptimalSizes();
	protected:
		void updateInternal(CM::UINT32 x, CM::UINT32 y, CM::UINT32 width, CM::UINT32 height, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);
	};
}