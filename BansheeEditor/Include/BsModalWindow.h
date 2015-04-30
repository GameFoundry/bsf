#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWindowBase.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT ModalWindow : public EditorWindowBase
	{
	public:
		virtual ~ModalWindow();

		virtual void update();
		virtual void close();
		void setTitle(const HString& title);

	protected:
		friend class EditorWindowManager;

		ModalWindow(const HString& title, bool hasCloseButton = false);
		Rect2I getContentArea() const;

		virtual void resized();

	private:
		void updateSize();
		UINT32 getTitleBarHeight() const;

		GUIPanel* mTitleBarPanel;
		GUIPanel* mTitleBarBgPanel;

		GUILabel* mTitle;
		GUIButton* mCloseButton;
		GUITexture* mTitleBarBg;

	protected:
		GUIPanel* mContents;
	};
}