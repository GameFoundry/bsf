#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIToggleGroup
	{
	public:
		~GUIToggleGroup();

		void add(GUIToggle* toggle);
		void remove(GUIToggle* toggle);

	private:
		friend class GUIToggle;

		GUIToggleGroup() {}

		void initialize(const std::shared_ptr<GUIToggleGroup>& sharedPtr);

		Vector<GUIToggle*>::type mButtons;
		std::weak_ptr<GUIToggleGroup> mThis;
	};
}