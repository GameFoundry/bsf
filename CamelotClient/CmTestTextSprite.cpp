#include "CmTestTextSprite.h"
#include "CmGameObject.h"
#include "CmRenderable.h"
#include "CmMesh.h"
#include "CmVector2.h"
#include "CmTextSprite.h"
#include "CmFont.h"
#include "CmMaterial.h"

namespace CamelotEngine
{
	TestTextSprite::TestTextSprite(GameObjectPtr parent)
		:Component(parent), mTextSprite(nullptr)
	{
		mTextRenderable = gameObject()->addComponent<Renderable>();
		mTextMesh = Mesh::create();

		mTextSprite = new TextSprite();
	}

	TestTextSprite::~TestTextSprite()
	{
		if(mTextSprite != nullptr)
			delete mTextSprite;
	}

	void TestTextSprite::setText(const String& text, FontHandle font, UINT32 fontSize, MaterialHandle textMaterial)
	{
		mTextSprite->setText(text);
		mTextSprite->setFont(font.getInternalPtr(), fontSize);

		UINT32 numTextFaces = mTextSprite->getNumFaces();
		UINT32 numVertices = numTextFaces * 4;
		UINT32 numIndices = numTextFaces * 6;

		std::shared_ptr<MeshData> textData(new MeshData());

		auto indices = new UINT32[numIndices];
		auto vertices = new Vector3[numVertices];
		auto uvs = new Vector2[numVertices];

		auto vec2Buffer = new Vector2[numVertices];

		mTextSprite->fillBuffer(vec2Buffer, uvs, indices, 0, numTextFaces);

		for(UINT32 i = 0; i < numVertices; i++)
			vertices[i] = Vector3(vec2Buffer[i].x, vec2Buffer[i].y, 0.0f);

		delete[] vec2Buffer;

		textData->setPositions(vertices, numVertices);
		textData->setUV0(uvs, numVertices);
		textData->setIndices(indices, numIndices);

		mTextMesh->setMeshData(textData);

		mTextRenderable->setMesh(mTextMesh);

		UINT32 nearestSize = font->getClosestAvailableSize(12);
		const FontData* fontData = font->getFontDataForSize(nearestSize);

		TextureHandle texturePage = fontData->texturePages[0]; // TODO - This won't work if font uses multiple pages
		textMaterial->setTexture("tex", texturePage);
		mTextRenderable->setMaterial(textMaterial);
	}

	void TestTextSprite::update()
	{

	}
}