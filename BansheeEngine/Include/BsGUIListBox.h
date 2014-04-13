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
		static const CM::String& getGUITypeName();

		static GUIListBox* create(const CM::Vector<CM::HString>::type& elements, const CM::String& styleName = CM::StringUtil::BLANK);
		static GUIListBox* create(const CM::Vector<CM::HString>::type& elements, const GUIOptions& layoutOptions, const CM::String& styleName = CM::StringUtil::BLANK);

		void setElements(const CM::Vector<CM::HString>::type& elements);

		virtual ElementType getElementType() const { return ElementType::ListBox; }

		boost::signal<void(CM::UINT32)> onSelectionChanged;
	protected:
		~GUIListBox();

	private:
		CM::UINT32 mSelectedIdx;
		CM::Vector<CM::HString>::type mElements;
		bool mIsListBoxOpen;

		GUIListBox(const CM::String& styleName, const CM::Vector<CM::HString>::type& elements, const GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const GUIMouseEvent& ev);

		void elementSelected(CM::UINT32 idx);

		void openListBox();
		void closeListBox();

		void onListBoxClosed();
	};
}