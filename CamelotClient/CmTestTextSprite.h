#include "BsPrerequisites.h"
#include "CmGUIWidget.h"

namespace CamelotEngine
{
	class TestTextSprite : public GUIWidget
	{
	protected:
		friend class SceneObject;

		TestTextSprite(const HSceneObject& parent);

		GUISkin* mSkin;
	public:
		~TestTextSprite();

		virtual void update();

		void setText(const HCamera& camera, const String& text, HFont font, UINT32 fontSize);
	};
}