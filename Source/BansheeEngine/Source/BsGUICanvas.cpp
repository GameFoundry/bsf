//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsGUICanvas.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsGUIDimensions.h"
#include "BsGUITexture.h"

namespace BansheeEngine
{
	const String& GUICanvas::getGUITypeName()
	{
		static String name = "Canvas";
		return name;
	}

	GUICanvas::GUICanvas(const String& styleName, const GUIDimensions& dimensions)
		:GUIElement(styleName, dimensions), mOutVertices(nullptr)
	{

	}

	GUICanvas::~GUICanvas()
	{
		clear();
	}

	GUICanvas* GUICanvas::create(const GUIOptions& options, const String& styleName)
	{
		return new (bs_alloc<GUICanvas>()) GUICanvas(getStyleName<GUICanvas>(styleName), GUIDimensions::create(options));
	}

	GUICanvas* GUICanvas::create(const String& styleName)
	{
		return new (bs_alloc<GUICanvas>()) GUICanvas(getStyleName<GUICanvas>(styleName), GUIDimensions::create());
	}

	void GUICanvas::drawLine(const Vector2I& a, const Vector2I& b, const Color& color)
	{
		mElements.push_back(CanvasElement());
		CanvasElement& element = mElements.back();

		element.type = CanvasElementType::Line;
		element.color = color;

		element.vertexStart = (UINT32)mVertexData.size();
		element.numVertices = 2;

		mVertexData.push_back(a);
		mVertexData.push_back(b);

		_markContentAsDirty();
	}

	void GUICanvas::drawPolyLine(const Vector<Vector2I>& vertices, const Color& color)
	{
		if(vertices.size() < 2)
		{
			LOGWRN("Invalid number of vertices. Ignoring call.");
			return;
		}

		mElements.push_back(CanvasElement());
		CanvasElement& element = mElements.back();

		element.type = CanvasElementType::Line;
		element.color = color;

		element.vertexStart = (UINT32)mVertexData.size();
		element.numVertices = (UINT32)vertices.size();

		mVertexData.insert(mVertexData.end(), vertices.begin(), vertices.end());
		_markContentAsDirty();
	}

	void GUICanvas::drawTexture(const HSpriteTexture& texture, const Rect2I& area, TextureScaleMode scaleMode, 
		const Color& color)
	{
		mElements.push_back(CanvasElement());
		CanvasElement& element = mElements.back();

		element.type = CanvasElementType::Image;
		element.color = color;
		element.imageDataId = (UINT32)mImageData.size();
		element.scaleMode = scaleMode;
		element.imageSprite = bs_new<ImageSprite>();

		mImageData.push_back({ texture, area });
		_markContentAsDirty();
	}

	void GUICanvas::drawTriangleStrip(const Vector<Vector2I>& vertices, const Color& color)
	{
		if (vertices.size() < 3)
		{
			LOGWRN("Invalid number of vertices. Ignoring call.");
			return;
		}

		mElements.push_back(CanvasElement());
		CanvasElement& element = mElements.back();

		element.type = CanvasElementType::Triangle;
		element.color = color;

		element.vertexStart = (UINT32)mVertexData.size();
		element.numVertices = (UINT32)(vertices.size() - 2) * 3;

		// Convert strip to list
		for(UINT32 i = 2; i < (UINT32)vertices.size(); i++)
		{
			mVertexData.push_back(vertices[i - 2]);
			mVertexData.push_back(vertices[i - 1]);
			mVertexData.push_back(vertices[i - 0]);
		}

		_markContentAsDirty();
	}

	void GUICanvas::drawTriangleList(const Vector<Vector2I>& vertices, const Color& color)
	{
		if (vertices.size() < 3 || vertices.size() % 3 != 0)
		{
			LOGWRN("Invalid number of vertices. Ignoring call.");
			return;
		}

		mElements.push_back(CanvasElement());
		CanvasElement& element = mElements.back();

		element.type = CanvasElementType::Triangle;
		element.color = color;

		element.vertexStart = (UINT32)mVertexData.size();
		element.numVertices = (UINT32)vertices.size();

		mVertexData.insert(mVertexData.end(), vertices.begin(), vertices.end());
		_markContentAsDirty();
	}

	void GUICanvas::drawText(const WString& text, const Vector2I& position, const HFont& font, UINT32 size, 
		const Color& color)
	{
		mElements.push_back(CanvasElement());
		CanvasElement& element = mElements.back();

		element.type = CanvasElementType::Text;
		element.color = color;
		element.textDataId = (UINT32)mTextData.size();
		element.size = size;
		element.textSprite = bs_new<TextSprite>();

		mTextData.push_back({ text, font, position });
		_markContentAsDirty();
	}

	void GUICanvas::clear()
	{
		bs_delete(mOutVertices);
		mOutVertices = nullptr;

		for (auto& element : mElements)
		{
			if(element.imageSprite != nullptr)
				bs_delete(element.imageSprite);
		}

		mElements.clear();

		mVertexData.clear();
		mImageData.clear();
		mTextData.clear();
	}

	UINT32 GUICanvas::_getNumRenderElements() const
	{
		return (UINT32)mElements.size();
	}

	const SpriteMaterialInfo& GUICanvas::_getMaterial(UINT32 renderElementIdx) const
	{
		static const SpriteMaterialInfo defaultMatInfo;

		const CanvasElement& element = mElements[renderElementIdx];
		switch (element.type)
		{
		case CanvasElementType::Line:
			// TODO
			return defaultMatInfo;
		case CanvasElementType::Image:
			return element.imageSprite->getMaterialInfo(renderElementIdx);
		case CanvasElementType::Text:
			return element.textSprite->getMaterialInfo(renderElementIdx);
		case CanvasElementType::Triangle:
			// TODO
			return defaultMatInfo;
		default:
			return defaultMatInfo;
		}
	}

	void GUICanvas::_getMeshSize(UINT32 renderElementIdx, UINT32& numVertices, UINT32& numIndices) const
	{
		const CanvasElement& element = mElements[renderElementIdx];
		switch (element.type)
		{
		case CanvasElementType::Line:
			// TODO
			numVertices = 0;
			numIndices = 0;
			break;
		case CanvasElementType::Image:
		{
			UINT32 numQuads = element.imageSprite->getNumQuads(renderElementIdx);
			numVertices = numQuads * 4;
			numIndices = numQuads * 6;
		}
		case CanvasElementType::Text:
		{
			UINT32 numQuads = element.textSprite->getNumQuads(renderElementIdx);
			numVertices = numQuads * 4;
			numIndices = numQuads * 6;
		}
		case CanvasElementType::Triangle:
			// TODO
			numVertices = 0;
			numIndices = 0;
			break;
		default:
			// TODO
			numVertices = 0;
			numIndices = 0;
			break;
		}
	}

	void GUICanvas::updateRenderElementsInternal()
	{
		bs_delete(mOutVertices);
		mOutVertices = nullptr;

		for(auto& element : mElements)
		{
			switch(element.type)
			{
			case CanvasElementType::Line:
				// TODO
				break;
			case CanvasElementType::Image:
				buildImageElement(element);
				break;
			case CanvasElementType::Text:
				buildTextElement(element);
				break;
			case CanvasElementType::Triangle:
				// TODO
				break;
			}
		}

		GUIElement::updateRenderElementsInternal();
	}

	void GUICanvas::buildImageElement(const CanvasElement& element)
	{
		assert(element.type == CanvasElementType::Image);

		const ImageElementData& imageData = mImageData[element.imageDataId];

		IMAGE_SPRITE_DESC desc;
		desc.width = imageData.area.width;
		desc.height = imageData.area.height;

		desc.transparent = true;
		desc.color = element.color;

		Vector2I textureSize;
		if (SpriteTexture::checkIsLoaded(imageData.texture))
		{
			desc.texture = imageData.texture.getInternalPtr();
			textureSize.x = desc.texture->getWidth();
			textureSize.y = desc.texture->getHeight();
		}

		Vector2I destSize(mLayoutData.area.width, mLayoutData.area.height);
		desc.uvScale = ImageSprite::getTextureUVScale(textureSize, destSize, element.scaleMode);

		element.imageSprite->update(desc, (UINT64)_getParentWidget());
	}

	void GUICanvas::buildTextElement(const CanvasElement& element)
	{
		assert(element.type == CanvasElementType::Text);

		const TextElementData& textData = mTextData[element.textDataId];

		TEXT_SPRITE_DESC desc;
		desc.font = textData.font;
		desc.fontSize = element.size;
		desc.text = textData.string;
		desc.color = element.color;

		element.textSprite->update(desc, (UINT64)_getParentWidget());
	}

	Vector2I GUICanvas::_getOptimalSize() const
	{
		return Vector2I(10, 10);
	}

	void GUICanvas::_fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 vertexOffset, UINT32 indexOffset,
		UINT32 maxNumVerts, UINT32 maxNumIndices, UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		const CanvasElement& element = mElements[renderElementIdx];

		Vector2I offset(mLayoutData.area.x, mLayoutData.area.y);
		Rect2I clipRect = mLayoutData.getLocalClipRect();
		switch(element.type)
		{
		case CanvasElementType::Line:
			// TODO - Handle offset and clipping, output vertices
			break;
		case CanvasElementType::Image:
		{
			const Rect2I& area = mImageData[element.imageDataId].area;

			offset.x += area.x;
			offset.y += area.y;
			clipRect.x -= area.x;
			clipRect.y -= area.y;

			element.imageSprite->fillBuffer(vertices, uv, indices, vertexOffset, indexOffset, maxNumVerts, maxNumIndices,
				vertexStride, indexStride, renderElementIdx, offset, clipRect);
		}
			break;
		case CanvasElementType::Text:
		{
			const Vector2I& position = mTextData[element.textDataId].position;
			offset += position;
			clipRect.x += position.x;
			clipRect.y += position.y;

			element.textSprite->fillBuffer(vertices, uv, indices, vertexOffset, indexOffset, maxNumVerts, maxNumIndices,
				vertexStride, indexStride, renderElementIdx, offset, mLayoutData.getLocalClipRect());
		}
			break;
		case CanvasElementType::Triangle:
			// TODO - Handle offset and clipping, output vertices
			break;
		}
	}
}