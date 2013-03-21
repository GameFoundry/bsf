#include "CmPrerequisites.h"
#include "CmComponent.h"

namespace CamelotEngine
{
	class TestTextSprite : public Component
	{
	private:
		friend class GameObject;

		TestTextSprite(GameObjectPtr parent);

		RenderablePtr mTextRenderable;
		MeshHandle mTextMesh;
		TextSprite* mTextSprite;

	public:
		~TestTextSprite();

		virtual void update();

		void setText(const String& text, FontHandle font, UINT32 fontSize, MaterialHandle textMaterial);
	};
}