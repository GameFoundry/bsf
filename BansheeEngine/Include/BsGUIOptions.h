#pragma once

#include "BsPrerequisites.h"

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

		static GUIOption fixedWidth(UINT32 value);
		static GUIOption flexibleWidth(UINT32 min = 0, UINT32 max = 0);

		static GUIOption fixedHeight(UINT32 value);
		static GUIOption flexibleHeight(UINT32 min = 0, UINT32 max = 0);

	private:
		friend struct GUILayoutOptions;

		UINT32 min, max;
		Type type;
	};

	class BS_EXPORT GUIOptions
	{
	public:
		GUIOptions() { }

		GUIOptions(const GUIOption& e0)
		{			
			mOptions.push_back(e0);					
		}

		GUIOptions(const GUIOption& e0, const GUIOption& e1)
		{
			mOptions.push_back(e0);
			mOptions.push_back(e1);
		}

		GUIOptions(const GUIOption& e0, const GUIOption& e1, const GUIOption& e2)
		{
			mOptions.push_back(e0);
			mOptions.push_back(e1);
			mOptions.push_back(e2);
		}

		GUIOptions(const GUIOption& e0, const GUIOption& e1, const GUIOption& e2,
			const GUIOption& e3)
		{
			mOptions.push_back(e0);
			mOptions.push_back(e1);
			mOptions.push_back(e2);
			mOptions.push_back(e3);
		}

		GUIOptions(const GUIOption& e0, const GUIOption& e1, const GUIOption& e2,
			const GUIOption& e4)
		{
			mOptions.push_back(e0);
			mOptions.push_back(e1);
			mOptions.push_back(e2);
			mOptions.push_back(e3);
			mOptions.push_back(e4);
		}

		void addOption(const GUIOption& option)
		{
			mOptions.push_back(option);
		}

	private:
		friend struct GUILayoutOptions;

		Vector<GUIOption>::type mOptions;
	};
}