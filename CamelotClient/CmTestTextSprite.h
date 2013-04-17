#include "BsPrerequisites.h"
#include "BsGUIWidget.h"

namespace CamelotFramework
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

		void setText(const BS::HCamera& camera, const String& text, HFont font, UINT32 fontSize);
	};
}