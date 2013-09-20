#include "BsPrerequisites.h"
#include "BsGUIWidget.h"
#include <boost/signal.hpp>

namespace BansheeEngine
{
	class BS_EXPORT GUIDropDownData
	{
		enum class Type
		{
			Separator,
			Entry,
			EntryExpandable
		};

	public:
		static GUIDropDownData separator();
		static GUIDropDownData button(const CM::WString& label, std::function<void()> callback, bool isExpandable = false);

		bool isSeparator() const { return mType == Type::Separator; }
		bool isExpandable() const { return mType == Type::EntryExpandable; }

		const CM::WString& getLabel() const { return mLabel; }
		std::function<void()> getCallback() const { return mCallback; }
	private:
		GUIDropDownData() { }

		std::function<void()> mCallback;
		CM::WString mLabel;
		Type mType; 
	};

	enum class GUIDropDownType
	{
		ListBox,
		ContextMenu,
		MenuBar
	};

	/**
	 * @brief	This is a generic GUI drop down box class that can be used for:
	 * 			list boxes, menu bars or context menus.
	 */
	class BS_EXPORT GUIDropDownBox : public GUIWidget
	{
	public:
		GUIDropDownBox(const CM::HSceneObject& parent);
		~GUIDropDownBox();

		void initialize(CM::Viewport* target, CM::RenderWindow* window, GUIElement* parentElem,
			const CM::Vector<GUIDropDownData>::type& elements, const GUISkin& skin, GUIDropDownType type);
	private:
		static const CM::UINT32 DROP_DOWN_BOX_WIDTH;

		CM::Vector<GUIDropDownData>::type mElements;
		CM::UINT32 mPage;
		CM::INT32 x, y;
		CM::UINT32 width, height;

		CM::Vector<GUITexture*>::type mCachedSeparators;
		CM::Vector<GUIButton*>::type mCachedEntryBtns;
		CM::Vector<GUIButton*>::type mCachedExpEntryBtns;
		GUIButton* mScrollUpBtn;
		GUIButton* mScrollDownBtn;
		GUITexture* mBackgroundFrame;

		const GUIElementStyle* mScrollUpStyle;
		const GUIElementStyle* mScrollDownStyle;
		const GUIElementStyle* mEntryBtnStyle;
		const GUIElementStyle* mEntryExpBtnStyle;
		const GUIElementStyle* mSeparatorStyle;
		const GUIElementStyle* mBackgroundStyle;
		SpriteTexturePtr mScrollUpBtnArrow;
		SpriteTexturePtr mScrollDownBtnArrow;

		GUIArea* mBackgroundArea;
		GUIArea* mContentArea;
		GUILayout* mContentLayout;

		void updateGUIElements();

		void scrollDown();
		void scrollUp();

		CM::UINT32 getElementHeight(CM::UINT32 idx) const;
	};
}