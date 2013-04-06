#include "CmPrerequisites.h"
#include "CmGUIWidget.h"

namespace CamelotEngine
{
	class TestTextSprite : public GUIWidget
	{
	private:
		friend class GameObject;

		TestTextSprite(GameObjectPtr parent);

		GUISkin* mSkin;
	public:
		~TestTextSprite();

		virtual void update();

		void setText(const CameraPtr& camera, const String& text, HFont font, UINT32 fontSize);
	};
}