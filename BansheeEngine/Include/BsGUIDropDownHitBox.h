#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	class GUIDropDownHitBox : public GUIElementContainer
	{
	public:
		static const String& getGUITypeName();

		static GUIDropDownHitBox* create(bool captureMouse);
		static GUIDropDownHitBox* create(bool captureMouse, const GUIOptions& layoutOptions);

		void setBounds(const RectI& bounds) { mBounds.clear(); mBounds.push_back(bounds); }
		void setBounds(const Vector<RectI>::type& bounds) { mBounds = bounds; }

		boost::signal<void()> onFocusLost;
		boost::signal<void()> onFocusGained;

	private:
		GUIDropDownHitBox(bool captureMouse, const GUILayoutOptions& layoutOptions);

		virtual bool commandEvent(const GUICommandEvent& ev);
		virtual bool mouseEvent(const GUIMouseEvent& ev);
		virtual bool _isInBounds(const Vector2I position) const;

		Vector<RectI>::type mBounds;
		bool mCaptureMouse;
	};
}