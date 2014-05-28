#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmModule.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Loosely based on ISO 639-1 two letter language codes
	 */
	enum class Language
	{
		Afar, 
		Abkhazian, 
		Avestan, 
		Afrikaans, 
		Akan, 
		Amharic, 
		Aragonese, 
		Arabic, 
		Assamese, 
		Avaric, 
		Aymara, 
		Azerbaijani, 
		Bashkir, 
		Belarusian, 
		Bulgarian, 
		Bihari, 
		Bislama, 
		Bambara, 
		Bengali, 
		Tibetan, 
		Breton, 
		Bosnian, 
		Catalan, 
		Chechen, 
		Chamorro, 
		Corsican, 
		Cree, 
		Czech, 
		ChurchSlavic,
		Chuvash, 
		Welsh, 
		Danish, 
		German, 
		Maldivian, 
		Bhutani, 
		Ewe, 		
		Greek, 
		EnglishUK, 
		EnglishUS,
		Esperanto, 
		Spanish, 
		Estonian, 
		Basque, 
		Persian, 
		Fulah, 
		Finnish, 
		Fijian, 
		Faroese, 
		French, 
		WesternFrisian, 
		Irish, 
		ScottishGaelic, 
		Galician, 
		Guarani, 
		Gujarati, 
		Manx, 
		Hausa, 
		Hebrew, 
		Hindi, 
		HiriMotu, 
		Croatian, 
		Haitian, 
		Hungarian, 
		Armenian, 
		Herero, 
		Interlingua, 
		Indonesian, 
		Interlingue, 
		Igbo, 
		SichuanYi, 
		Inupiak, 
		Ido, 
		Icelandic, 
		Italian, 
		Inuktitut, 
		Japanese, 
		Javanese, 
		Georgian, 
		Kongo, 
		Kikuyu, 
		Kuanyama, 
		Kazakh, 
		Kalaallisut, 
		Cambodian, 
		Kannada, 
		Korean, 
		Kanuri, 
		Kashmiri, 
		Kurdish, 
		Komi, 
		Cornish, 
		Kirghiz, 
		Latin, 
		Luxembourgish, 
		Ganda, 
		Limburgish,
		Lingala, 
		Laotian, 
		Lithuanian, 
		LubaKatanga, 
		Latvian,
		Malagasy, 
		Marshallese, 
		Maori, 
		Macedonian, 
		Malayalam, 
		Mongolian, 
		Moldavian, 
		Marathi, 
		Malay, 
		Maltese, 
		Burmese, 
		Nauru, 
		NorwegianBokmal, 
		Ndebele, 
		Nepali, 
		Ndonga, 
		Dutch, 
		NorwegianNynorsk, 
		Norwegian, 
		Navaho, 
		Nyanja, 
		Provençal, 
		Ojibwa, 
		Oromo, 
		Oriya, 
		Ossetic, 
		Punjabi, 
		Pali, 
		Polish, 
		Pushto, 
		Portuguese, 
		Quechua, 
		Romansh, 
		Kirundi, 
		Romanian, 
		Russian, 
		Kinyarwanda, 
		Sanskrit, 
		Sardinian, 
		Sindhi, 
		NorthernSami, 
		Sangro, 
		Sinhalese, 
		Slovak, 
		Slovenian, 
		Samoan, 
		Shona, 
		Somali, 
		Albanian, 
		Serbian, 
		Swati,
		Sesotho,
		Sundanese, 
		Swedish, 
		Swahili, 
		Tamil, 
		Telugu, 
		Tajik, 
		Thai, 
		Tigrinya, 
		Turkmen, 
		Tagalog, 
		Setswana, 
		Tonga, 
		Turkish, 
		Tsonga, 
		Tatar,
		Twi, 
		Tahitian, 
		Uighur, 
		Ukrainian, 
		Urdu, 
		Uzbek, 
		Venda, 
		Vietnamese, 
		Volapuk, 
		Walloon, 
		Wolof, 
		Xhosa, 
		Yiddish, 
		Yoruba, 
		Zhuang,
		Chinese,
		Zulu,
		Count // Number of entries
	};

	/**
	 * @brief	Internal data used for representing a localized string instance.
	 * 			e.g. a specific instance of a localized string using specific parameters.
	 */
	struct LocalizedStringData
	{
		struct Common
		{
			WString identifier;
			Event<void()> onStringDataModified;
		};

		struct ParamOffset
		{
			ParamOffset()
				:paramIdx(0), location(0)
			{ }

			ParamOffset(UINT32 _paramIdx, UINT32 _location)
				:paramIdx(_paramIdx), location(_location)
			{ }

			UINT32 paramIdx;
			UINT32 location;
		};

		LocalizedStringData();
		~LocalizedStringData();

		WString string;
		UINT32 numParameters;
		ParamOffset* parameterOffsets; 

		Common* commonData;

		void concatenateString(WString& outputString, WString* parameters, UINT32 numParameterValues) const;
		void updateString(const WString& string);
	};

	/**
	 * @brief	Class that handles string localization. Stores strings and their translations
	 * 			in various languages, along with the ability to switch currently active language.
	 */
	class BS_UTILITY_EXPORT StringTable : public Module<StringTable>
	{
		// TODO - When editing string table I will need to ensure that all languages of the same string have the same number of parameters

		struct LanguageData
		{
			UnorderedMap<WString, LocalizedStringData*> strings;
		};
	public:
		StringTable();
		~StringTable();

		/**
		 * @brief	Gets the currently active language.
		 */
		Language getActiveLanguage() const { return mActiveLanguage; }

		/**
		 * @brief	Changes the currently active language.
		 * 			This will update any localized string instances.
		 */
		void setActiveLanguage(Language language);

		/**
		 * @brief	Adds or modifies string translation for the specified language.
		 */
		void setString(const WString& identifier, Language language, const WString& string);

		/**
		 * @brief	Removes the string described by identifier, from all languages.
		 */
		void removeString(const WString& identifier);

		/**
		 * @brief	Gets a string data for the specified string identifier and currently active language.
		 *
		 * @param	identifier		   	Unique string identifier.
		 * @param	insertIfNonExisting	If true, a new string data for the specified identifier and language will be
		 * 								added to the table if data doesn't already exist. The data will use the identifier as
		 * 								the translation string.
		 *
		 * @return	The string data. Don't store reference to this data as it may get deleted.
		 */
		LocalizedStringData& getStringData(const WString& identifier, bool insertIfNonExisting = true);

		/**
		 * @brief	Gets a string data for the specified string identifier and language.
		 *
		 * @param	identifier		   	Unique string identifier.
		 * @param	language		   	Language.
		 * @param	insertIfNonExisting	If true, a new string data for the specified identifier and language will be
		 * 								added to the table if data doesn't already exist. The data will use the identifier as
		 * 								the translation string.
		 *
		 * @return	The string data. Don't store reference to this data as it may get deleted.
		 */
		LocalizedStringData& getStringData(const WString& identifier, Language language, bool insertIfNonExisting = true);

	private:
		friend class HString;

		static const Language DEFAULT_LANGUAGE;

		Language mActiveLanguage;
		LanguageData* mActiveLanguageData;
		LanguageData* mDefaultLanguageData;

		LanguageData* mAllLanguages;

		UnorderedMap<WString, LocalizedStringData::Common*> mCommonData;

		void notifyAllStringsChanged();
	};
}