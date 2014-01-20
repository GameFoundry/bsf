#include "BsEditorPrerequisites.h"
#include "BsGUIWidget.h"

namespace BansheeEditor
{
	class TestTextSprite : public BS::GUIWidget
	{
	protected:
		friend class CM::SceneObject;

		TestTextSprite(const CM::HSceneObject& parent, CM::Viewport* target);
	public:
		~TestTextSprite();

		virtual void update();

		void init(const BS::HCamera& camera, const CM::String& text, CM::RenderTexturePtr sceneView);

	private:
		BS::GUILabel* mLabel;
		BS::GUIListBox* mListBox;
		GUISceneTreeView* mSceneTreeView;
		CM::HString labelString;

		void dbgBtn();
	};
}