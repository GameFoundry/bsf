#include "CmTextureView.h"
#include "CmTexture.h"
#include "CmUtil.h"

namespace BansheeEngine
{
	size_t TextureView::HashFunction::operator()(const TEXTURE_VIEW_DESC &key) const
	{
		size_t seed = 0;
		hash_combine(seed, key.mostDetailMip);
		hash_combine(seed, key.numMips);
		hash_combine(seed, key.firstArraySlice);
		hash_combine(seed, key.numArraySlices);
		hash_combine(seed, key.usage);

		return seed;
	}

	bool TextureView::EqualFunction::operator()
		(const TEXTURE_VIEW_DESC &a, const TEXTURE_VIEW_DESC &b) const
	{
		return a.mostDetailMip == b.mostDetailMip && a.numMips == b.numMips 
			&& a.firstArraySlice == b.firstArraySlice && a.numArraySlices == b.numArraySlices && a.usage == b.usage;
	}

	TextureView::TextureView()
	{ }

	TextureView::~TextureView()
	{ }

	void TextureView::initialize(TexturePtr texture, TEXTURE_VIEW_DESC& _desc)
	{
		mOwnerTexture = texture;
		mDesc = _desc;

		CoreObject::initialize();
	}
}