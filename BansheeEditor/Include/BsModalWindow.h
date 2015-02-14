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

		static ModalWindow* create();

	protected:
		friend class EditorWindowManager;

		ModalWindow();

		virtual void resized();
	private:
		void updateSize();
	};
}