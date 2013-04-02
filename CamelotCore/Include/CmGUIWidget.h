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

	protected:
		GUILabel* addLabel(const String& text);

		const GUISkin* getGUISkin() const;

	private:
		friend class GameObject;
		friend class GUIManager;

		GUIWidget(GameObjectPtr parent);

		vector<GUIElement*>::type mElements;
		/**
		 * @brief	GUIWidgets sharing the same mesh group ID will attempted to be merged into
		 * 			a single render mesh (as much as materials allow).
		 * 			
		 * @note	ID of -1 means that the widget mesh should never be grouped.
		 */
		INT32 mMeshGroupID;
		const GUISkin* mSkin;
		static GUISkin DefaultSkin;
	};
}