#pragma once

#include "BsPrerequisites.h"
#include <boost/preprocessor.hpp>

namespace BansheeEngine
{
	class BS_EXPORT GUIOption
	{
		enum class Type
		{
			FixedWidth,
			FlexibleWidth,
			FixedHeight,
			FlexibleHeight
		};

	public:
		GUIOption();

		static GUIOption fixedWidth(CM::UINT32 value);
		static GUIOption flexibleWidth(CM::UINT32 min = 0, CM::UINT32 max = 0);

		static GUIOption fixedHeight(CM::UINT32 value);
		static GUIOption flexibleHeight(CM::UINT32 min = 0, CM::UINT32 max = 0);

	private:
		friend struct GUILayoutOptions;

		CM::UINT32 min, max;
		Type type;
	};

	class BS_EXPORT GUIOptions
	{
	public:
		GUIOptions() { }

		// Constructor that accepts an arbitrary number of parameters > 0
#define MAKE_GUIOPTION_CONSTRUCTOR(z, n, unused)											\
	GUIOptions(BOOST_PP_ENUM_PARAMS(n, const GUIOption& e) )	\
	{			\
			mOptions.push_back(e0);					\
			BOOST_PP_REPEAT_FROM_TO(1, n, ADD_OPTION, ~) 									\
	}

#define ADD_OPTION(z, i, unused) \
	mOptions.push_back(BOOST_PP_CAT(e, i));

	BOOST_PP_REPEAT_FROM_TO(1, 15, MAKE_GUIOPTION_CONSTRUCTOR, ~)

#undef ADD_OPTION
#undef MAKE_GUIOPTION_CONSTRUCTOR

		void addOption(const GUIOption& option)
		{
			mOptions.push_back(option);
		}

	private:
		friend struct GUILayoutOptions;

		CM::Vector<GUIOption>::type mOptions;
	};
}