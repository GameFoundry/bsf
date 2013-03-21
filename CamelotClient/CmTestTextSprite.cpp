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
		textData->declaration->addElement(0, 0, VET_FLOAT3, VES_POSITION);
		textData->declaration->addElement(0, 12, VET_FLOAT2, VES_TEXCOORD);

		textData->index = new int[numIndices];
		textData->indexCount = numIndices;
		textData->vertexCount = numVertices;
		
		std::shared_ptr<MeshData::VertexData> vertData(new MeshData::VertexData(numVertices));
		vertData->vertex = new Vector3[numVertices];
		vertData->uv0 = new Vector2[numVertices];

		Vector2* vec2Buffer = new Vector2[numVertices];

		mTextSprite->fillBuffer(vec2Buffer, vertData->uv0, (UINT32*)textData->index, 0, numTextFaces);

		for(UINT32 i = 0; i < numVertices; i++)
		{
			vertData->vertex[i] = Vector3(vec2Buffer[i].x, vec2Buffer[i].y, 0.0f);
		}

		textData->vertexBuffers[0] = vertData;

		MeshData::SubMeshData subMeshData;
		subMeshData.indexOffset = 0;
		subMeshData.indexCount = numIndices;
		textData->subMeshes.push_back(subMeshData);

		mTextMesh->setMeshData(textData);

		delete[] vec2Buffer;

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