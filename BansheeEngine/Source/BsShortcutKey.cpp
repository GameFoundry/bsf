#include "BsShortcutKey.h"
#include "BsPlatform.h"

namespace BansheeEngine
{
	const ShortcutKey ShortcutKey::NONE = ShortcutKey();

	inline size_t ShortcutKey::Hash::operator()(const ShortcutKey& x) const
	{
		size_t seed = 0;
		hash_combine(seed, (UINT32)x.button);
		hash_combine(seed, (UINT32)x.modifier);

		return seed;
	}

	inline bool ShortcutKey::Equals::operator()(const ShortcutKey& a, const ShortcutKey& b) const
	{
		return a.button == b.button && a.modifier == b.modifier;
	}

	ShortcutKey::ShortcutKey()
		:modifier(ButtonModifier::None), button(BC_UNASSIGNED)
	{ }

	ShortcutKey::ShortcutKey(ButtonModifier modifier, ButtonCode code)
		: modifier(modifier), button(code)
	{ }

	WString ShortcutKey::getName() const
	{
		static const WString MODIFIER_TO_NAME[8] =
		{
			L"", L"Shift + ", L"Ctrl + ", L"Shift + Ctrl + ",
			L"Alt + ", L"Shift + Alt + ", L"Ctrl + Alt + ",
			L"Shift + Ctrl + Alt + "
		};

		if (button == BC_UNASSIGNED)
			return L"";

		WString charStr = Platform::keyCodeToUnicode((UINT32)button);
		StringUtil::toUpperCase(charStr);

		return MODIFIER_TO_NAME[(UINT32)modifier] + charStr;
	}
}