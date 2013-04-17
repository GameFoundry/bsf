#include "BsPrerequisites.h"
#include "BsGUIWidget.h"

namespace CamelotEngine
{
	class TestTextSprite : public BansheeEngine::GUIWidget
	{
	protected:
		friend class CM::SceneObject;

		TestTextSprite(const HSceneObject& parent);

		BansheeEngine::GUISkin* mSkin;
	public:
		~TestTextSprite();

		virtual void update();

		void setText(const HCamera& camera, const String& text, HFont font, UINT32 fontSize);
	};
}