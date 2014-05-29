#include "BsEditorPrerequisites.h"
#include "BsGUIWidget.h"

namespace BansheeEngine
{
	class TestTextSprite : public GUIWidget
	{
	protected:
		friend class SceneObject;

		TestTextSprite(const HSceneObject& parent, Viewport* target);
	public:
		~TestTextSprite();

		virtual void update();

		void init(const HCamera& camera, const String& text, RenderTexturePtr sceneView);

	private:
		GUISceneTreeView* mSceneTreeView;
		GUIResourceTreeView* mResourceTreeView;
	};
}