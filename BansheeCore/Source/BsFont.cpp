#include "BsFont.h"
#include "BsFontRTTI.h"
#include "BsFontManager.h"
#include "BsResources.h"

namespace BansheeEngine
{
	const CHAR_DESC& FontData::getCharDesc(UINT32 charId) const
	{
		auto iterFind = fontDesc.characters.find(charId);
		if(iterFind != fontDesc.characters.end())
		{
			return fontDesc.characters.at(charId);
		}

		return fontDesc.missingGlyph;
	}

	RTTITypeBase* FontData::getRTTIStatic()
	{
		return FontDataRTTI::instance();
	}

	RTTITypeBase* FontData::getRTTI() const
	{
		return FontData::getRTTIStatic();
	}

	Font::Font()
		:Resource(false)
	{ }

	Font::~Font()
	{ }

	void Font::initialize(const Vector<FontData>& fontData)
	{
		for(auto iter = fontData.begin(); iter != fontData.end(); ++iter)
			mFontDataPerSize[iter->size] = *iter;

		Resource::initialize();
	}

	const FontData* Font::getFontDataForSize(UINT32 size) const
	{
		auto iterFind = mFontDataPerSize.find(size);

		if(iterFind == mFontDataPerSize.end())
			return nullptr;

		return &iterFind->second;
	}

	INT32 Font::getClosestAvailableSize(UINT32 size) const
	{
		UINT32 minDiff = std::numeric_limits<UINT32>::max();
		UINT32 bestSize = size;

		for(auto iter = mFontDataPerSize.begin(); iter != mFontDataPerSize.end(); ++iter)
		{
			if(iter->first == size)
				return size;
			else if(iter->first > size)
			{
				UINT32 diff = iter->first - size;
				if(diff < minDiff)
				{
					minDiff = diff;
					bestSize = iter->first;
				}
			}
			else
			{
				UINT32 diff = size - iter->first;
				if(diff < minDiff)
				{
					minDiff = diff;
					bestSize = iter->first;
				}
			}
		}

		return bestSize;
	}

	void Font::getResourceDependencies(FrameVector<HResource>& dependencies) const
	{
		for (auto& fontDataEntry : mFontDataPerSize)
		{
			for (auto& texture : fontDataEntry.second.texturePages)
			{
				if (texture != nullptr)
					dependencies.push_back(texture);
			}
		}
	}

	void Font::getCoreDependencies(FrameVector<SPtr<CoreObject>>& dependencies)
	{
		for (auto& fontDataEntry : mFontDataPerSize)
		{
			for (auto& texture : fontDataEntry.second.texturePages)
			{
				if (texture.isLoaded())
					dependencies.push_back(texture.getInternalPtr());
			}
		}
	}

	HFont Font::create(const Vector<FontData>& fontData)
	{
		FontPtr newFont = _createPtr(fontData);

		return static_resource_cast<Font>(gResources()._createResourceHandle(newFont));
	}

	FontPtr Font::_createPtr(const Vector<FontData>& fontData)
	{
		return FontManager::instance().create(fontData);
	}

	RTTITypeBase* Font::getRTTIStatic()
	{
		return FontRTTI::instance();
	}

	RTTITypeBase* Font::getRTTI() const
	{
		return Font::getRTTIStatic();
	}
}