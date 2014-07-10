//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Controls GUI element properties, possibly by overriding
	 *			the default properties specified in GUI element style.
	 */
	class BS_EXPORT GUIOption
	{
		/**
		 * @brief	Type of GUI element options.
		 */
		enum class Type
		{
			FixedWidth,
			FlexibleWidth,
			FixedHeight,
			FlexibleHeight
		};

	public:
		GUIOption();

		/**
		 * @brief	Constructs a GUI option notifying the GUI layout that this element has a fixed width.
		 *			This will override width property set in element style.
		 */
		static GUIOption fixedWidth(UINT32 value);

		/**
		 * @brief	Constructs a GUI option notifying the GUI layout that this element has a flexible width with
		 *			optional min/max constraints (value of 0 means no constraint). This will override width property 
		 *			set in element style.
		 */
		static GUIOption flexibleWidth(UINT32 min = 0, UINT32 max = 0);

		/**
		 * @brief	Constructs a GUI option notifying the GUI layout that this element has a fixed height.
		 *			This will override height property set in element style.
		 */
		static GUIOption fixedHeight(UINT32 value);

		/**
		 * @brief	Constructs a GUI option notifying the GUI layout that this element has a flexible height with
		 *			optional min/max constraints (value of 0 means no constraint). This will override height property 
		 *			set in element style.
		 */
		static GUIOption flexibleHeight(UINT32 min = 0, UINT32 max = 0);

	private:
		friend struct GUILayoutOptions;

		UINT32 min, max;
		Type type;
	};

	/**
	 * @brief	Container for a list of options used for controlling GUI element properties.
	 */
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
			const GUIOption& e3, const GUIOption& e4)
		{
			mOptions.push_back(e0);
			mOptions.push_back(e1);
			mOptions.push_back(e2);
			mOptions.push_back(e3);
			mOptions.push_back(e4);
		}

		/**
		 * @brief	Adds a new option to the options list. 
		 */
		void addOption(const GUIOption& option)
		{
			mOptions.push_back(option);
		}

	private:
		friend struct GUILayoutOptions;

		Vector<GUIOption> mOptions;
	};
}