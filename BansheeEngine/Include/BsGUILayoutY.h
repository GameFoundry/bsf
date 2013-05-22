#pragma once

#include "BsPrerequisites.h"
#include "BsGUILayout.h"

namespace BansheeEngine
{
	class BS_EXPORT GUILayoutY : public GUILayout
	{
	public:
		GUILayoutY() {};
		~GUILayoutY() {};

	protected:
		void updateOptimalSizes();
		void updateInternal(UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT32 depth);
	};
}