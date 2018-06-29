//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Image/BsSpriteTexture.h"
#include "Private/RTTI/BsSpriteTextureRTTI.h"
#include "Image/BsTexture.h"
#include "Resources/BsResources.h"
#include "Resources/BsBuiltinResources.h"

namespace bs
{
	Rect2 SpriteTextureBase::evaluate(float t) const
	{
		if(mPlayback == SpriteAnimationPlayback::None)
			return Rect2(mUVOffset.x, mUVOffset.y, mUVScale.x, mUVScale.y);

		// Note: Duration could be pre-calculated
		float duration = 0.0f;
		if (mAnimation.fps > 0)
			duration = mAnimation.count / (float)mAnimation.fps;

		switch(mPlayback)
		{
		default:
		case SpriteAnimationPlayback::Normal:
			t = Math::clamp(t, 0.0f, duration);
			break;
		case SpriteAnimationPlayback::Loop: 
			t = Math::repeat(t, duration);
			break;
		case SpriteAnimationPlayback::PingPong: 
			t = Math::pingPong(t, duration);
			break;
		}

		const float pct = t / duration;
		UINT32 frame = 0;
		
		if(mAnimation.count > 0)
			frame = Math::clamp(Math::floorToPosInt(pct * mAnimation.count), 0U, mAnimation.count - 1);

		const UINT32 row = frame / mAnimation.numRows;
		const UINT32 column = frame % mAnimation.numColumns;

		Rect2 output;

		// Note: These could be pre-calculated
		output.width = mUVScale.x / mAnimation.numColumns;
		output.height = mUVScale.y / mAnimation.numRows;

		output.x = mUVOffset.x + column * output.width;
		output.y = mUVOffset.y + row * output.height;

		return output;
	}

	SpriteTexture::SpriteTexture(const Vector2& uvOffset, const Vector2& uvScale, const HTexture& texture)
		:SpriteTextureBase(uvOffset, uvScale), mAtlasTexture(texture)
	{ }

	const HSpriteTexture& SpriteTexture::dummy()
	{
		return BuiltinResources::instance().getDummySpriteTexture();
	}

	bool SpriteTexture::checkIsLoaded(const HSpriteTexture& tex)
	{
		return tex != nullptr && tex.isLoaded(false) && tex->getTexture() != nullptr && tex->getTexture().isLoaded(false);
	}

	UINT32 SpriteTexture::getWidth() const
	{
		return Math::roundToInt(mAtlasTexture->getProperties().getWidth() * mUVScale.x);
	}

	UINT32 SpriteTexture::getHeight() const
	{
		return Math::roundToInt(mAtlasTexture->getProperties().getHeight() * mUVScale.y);
	}

	void SpriteTexture::_markCoreDirty()
	{
		markCoreDirty();
	}

	SPtr<ct::CoreObject> SpriteTexture::createCore() const
	{
		SPtr<ct::Texture> texturePtr;
		if(mAtlasTexture.isLoaded())
			texturePtr = mAtlasTexture->getCore();

		ct::SpriteTexture* spriteTexture = new (bs_alloc<ct::SpriteTexture>()) ct::SpriteTexture(mUVOffset, mUVScale, 
			std::move(texturePtr), mAnimation, mPlayback);

		SPtr<ct::SpriteTexture> spriteTexPtr = bs_shared_ptr<ct::SpriteTexture>(spriteTexture);
		spriteTexPtr->_setThisPtr(spriteTexPtr);

		return spriteTexPtr;
	}

	CoreSyncData SpriteTexture::syncToCore(FrameAlloc* allocator)
	{
		UINT32 size = rttiGetElemSize(mUVOffset) + rttiGetElemSize(mUVScale) + sizeof(SPtr<ct::Texture>) + 
			rttiGetElemSize(mAnimation) + rttiGetElemSize(mPlayback);

		UINT8* buffer = allocator->alloc(size);
		char* dataPtr = (char*)buffer;

		dataPtr = rttiWriteElem(mUVOffset, dataPtr);
		dataPtr = rttiWriteElem(mUVScale, dataPtr);
		
		SPtr<ct::Texture>* texture = new (dataPtr) SPtr<ct::Texture>();
		if (mAtlasTexture.isLoaded(false))
			*texture = mAtlasTexture->getCore();
		else
			*texture = nullptr;

		dataPtr += sizeof(SPtr<ct::Texture>);

		dataPtr = rttiWriteElem(mAnimation, dataPtr);
		dataPtr = rttiWriteElem(mPlayback, dataPtr);

		return CoreSyncData(buffer, size);
	}

	void SpriteTexture::getResourceDependencies(FrameVector<HResource>& dependencies) const
	{
		if (mAtlasTexture != nullptr)
			dependencies.push_back(mAtlasTexture);
	}

	void SpriteTexture::getCoreDependencies(Vector<CoreObject*>& dependencies)
	{
		if (mAtlasTexture.isLoaded())
			dependencies.push_back(mAtlasTexture.get());
	}

	SPtr<ct::SpriteTexture> SpriteTexture::getCore() const
	{
		return std::static_pointer_cast<ct::SpriteTexture>(mCoreSpecific);
	}

	HSpriteTexture SpriteTexture::create(const HTexture& texture)
	{
		SPtr<SpriteTexture> texturePtr = _createPtr(texture);

		return static_resource_cast<SpriteTexture>(gResources()._createResourceHandle(texturePtr));
	}

	HSpriteTexture SpriteTexture::create(const Vector2& uvOffset, const Vector2& uvScale, const HTexture& texture)
	{
		SPtr<SpriteTexture> texturePtr = _createPtr(uvOffset, uvScale, texture);

		return static_resource_cast<SpriteTexture>(gResources()._createResourceHandle(texturePtr));
	}

	SPtr<SpriteTexture> SpriteTexture::_createPtr(const HTexture& texture)
	{
		SPtr<SpriteTexture> texturePtr = bs_core_ptr<SpriteTexture>
			(new (bs_alloc<SpriteTexture>()) SpriteTexture(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f), texture));

		texturePtr->_setThisPtr(texturePtr);
		texturePtr->initialize();

		return texturePtr;
	}

	SPtr<SpriteTexture> SpriteTexture::_createPtr(const Vector2& uvOffset, const Vector2& uvScale, const HTexture& texture)
	{
		SPtr<SpriteTexture> texturePtr = bs_core_ptr<SpriteTexture>
			(new (bs_alloc<SpriteTexture>()) SpriteTexture(uvOffset, uvScale, texture));

		texturePtr->_setThisPtr(texturePtr);
		texturePtr->initialize();

		return texturePtr;
	}

	SPtr<SpriteTexture> SpriteTexture::createEmpty()
	{
		SPtr<SpriteTexture> texturePtr = bs_core_ptr<SpriteTexture>
			(new (bs_alloc<SpriteTexture>()) SpriteTexture(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f), HTexture()));

		texturePtr->_setThisPtr(texturePtr);
		texturePtr->initialize();

		return texturePtr;
	}

	RTTITypeBase* SpriteTexture::getRTTIStatic()
	{
		return SpriteTextureRTTI::instance();
	}

	RTTITypeBase* SpriteTexture::getRTTI() const
	{
		return SpriteTexture::getRTTIStatic();
	}

	namespace ct
	{
		SpriteTexture::SpriteTexture(const Vector2& uvOffset, const Vector2& uvScale, SPtr<Texture> texture,
			const SpriteSheetGridAnimation& anim, SpriteAnimationPlayback playback)
			:SpriteTextureBase(uvOffset, uvScale), mAtlasTexture(std::move(texture))
		{
			mAnimation = anim;
			mPlayback = playback;
		}

		void SpriteTexture::syncToCore(const CoreSyncData& data)
		{
			char* dataPtr = (char*)data.getBuffer();

			dataPtr = rttiReadElem(mUVOffset, dataPtr);
			dataPtr = rttiReadElem(mUVScale, dataPtr);

			SPtr<Texture>* texture = (SPtr<Texture>*)dataPtr;

			mAtlasTexture = *texture;
			texture->~SPtr<Texture>();
			dataPtr += sizeof(SPtr<Texture>);

			dataPtr = rttiReadElem(mAnimation, dataPtr);
			dataPtr = rttiReadElem(mPlayback, dataPtr);
		}
	}
}