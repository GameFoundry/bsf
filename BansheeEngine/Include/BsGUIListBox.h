#pragma once

#include "BsPrerequisites.h"
#include "BsGUIButtonBase.h"
#include "BsImageSprite.h"
#include "BsTextSprite.h"
#include "boost/signal.hpp"

namespace BansheeEngine
{
	class BS_EXPORT GUIListBox : public GUIButtonBase
	{
	public:
		static const String& getGUITypeName();

		static GUIListBox* create(const Vector<HString>::type& elements, const String& styleName = StringUtil::BLANK);
		static GUIListBox* create(const Vector<HString>::type& elements, const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

		void setElements(const Vector<HString>::type& elements);

		virtual ElementType getElementType() const { return ElementType::ListBox; }

		boost::signal<void(UINT32)> onSelectionChanged;
	protected:
		~GUIListBox();

	private:
		UINT32 mSelectedIdx;
		Vector<HString>::type mElements;
		bool mIsListBoxOpen;

		GUIListBox(const String& styleName, const Vector<HString>::type& elements, const GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const GUIMouseEvent& ev);

		void elementSelected(UINT32 idx);

		void openListBox();
		void closeListBox();

		void onListBoxClosed();
	};
}