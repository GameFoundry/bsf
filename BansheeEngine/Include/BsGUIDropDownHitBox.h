#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	class GUIDropDownHitBox : public GUIElementContainer
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIDropDownHitBox* create(bool captureMouse, const GUIElementStyle* style = nullptr);
		static GUIDropDownHitBox* create(bool captureMouse, const GUIOptions& layoutOptions, const GUIElementStyle* style = nullptr);

		void setBounds(const CM::RectI& bounds) { mBounds.clear(); mBounds.push_back(bounds); }
		void setBounds(const CM::Vector<CM::RectI>::type& bounds) { mBounds = bounds; }

		boost::signal<void()> onFocusLost;
		boost::signal<void()> onFocusGained;

	private:
		GUIDropDownHitBox(bool captureMouse, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions);

		virtual bool commandEvent(const GUICommandEvent& ev);
		virtual bool mouseEvent(const GUIMouseEvent& ev);
		virtual bool _isInBounds(const CM::Vector2I position) const;

		CM::Vector<CM::RectI>::type mBounds;
		bool mCaptureMouse;
	};
}