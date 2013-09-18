#include "BsPrerequisites.h"
#include "BsGUIWidget.h"
#include <boost/signal.hpp>

namespace BansheeEngine
{
	class BS_EXPORT GUIDropDownBox : public GUIWidget
	{
	public:
		GUIDropDownBox(const CM::HSceneObject& parent);
		~GUIDropDownBox();

		void initialize(CM::Viewport* target, CM::RenderWindow* window, GUIDropDownList* parentList,
			const CM::Vector<CM::WString>::type& elements, std::function<void(CM::UINT32)> selectedCallback);
	private:
		static const CM::UINT32 DROP_DOWN_BOX_WIDTH;

		std::function<void(CM::UINT32)> mSelectedDropDownEntryCallback;
		CM::Vector<CM::WString>::type mDropDownElements;
		CM::Vector<GUIButton*>::type mDropDownElementButtons;
		CM::UINT32 mDropDownStartIdx;

		void scrollDown();
		void scrollUp();
		void entrySelected(CM::UINT32 idx);
	};
}