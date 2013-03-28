#pragma once

#include "CmPrerequisites.h"
#include "CmComponent.h"

namespace CamelotEngine
{
	class CM_EXPORT GUIWidget : public Component
	{
	public:
		~GUIWidget();

		virtual void update();

	private:
		friend class GameObject;

		GUI* mGUI;

		GUIWidget(GameObjectPtr parent);
	};
}