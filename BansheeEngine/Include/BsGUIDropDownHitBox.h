#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	class GUIDropDownHitBox : public GUIElementContainer
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIDropDownHitBox* create(GUIWidget& parent, const GUIElementStyle* style = nullptr);
		static GUIDropDownHitBox* create(GUIWidget& parent, const GUIOptions& layoutOptions, const GUIElementStyle* style = nullptr);

		void setBounds(const CM::Vector<CM::RectI>::type& bounds) { mBounds = bounds; }

		boost::signal<void()> onFocusLost;
		boost::signal<void()> onFocusGained;

	private:
		GUIDropDownHitBox(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions);

		virtual bool commandEvent(const GUICommandEvent& ev);
		virtual bool _isInBounds(const CM::Vector2I position) const;

		CM::Vector<CM::RectI>::type mBounds;
	};
}