#pragma once

#include "BsPrerequisites.h"
#include "BsGUIButtonBase.h"
#include "BsImageSprite.h"
#include "BsTextSprite.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIListBox : public GUIButtonBase
	{
	public:
		static const String& getGUITypeName();

		static GUIListBox* create(const Vector<HString>& elements, const String& styleName = StringUtil::BLANK);
		static GUIListBox* create(const Vector<HString>& elements, const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

		void setElements(const Vector<HString>& elements);

		virtual ElementType getElementType() const { return ElementType::ListBox; }

		Event<void(UINT32)> onSelectionChanged;
	protected:
		~GUIListBox();

	private:
		UINT32 mSelectedIdx;
		Vector<HString> mElements;
		bool mIsListBoxOpen;

		GUIListBox(const String& styleName, const Vector<HString>& elements, const GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const GUIMouseEvent& ev);

		void elementSelected(UINT32 idx);

		void openListBox();
		void closeListBox();

		void onListBoxClosed();
	};
}