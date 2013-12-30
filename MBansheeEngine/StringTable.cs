using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class StringTable
    {
        public Language GetActiveLanguage()
        {
            Language value;
            Internal_GetActiveLanguage(out value);
            return value;
        }

		public void SetActiveLanguage(Language language)
		{
            Internal_SetActiveLanguage(language);
		}

		public void SetString(string identifier, Language language, string value)
		{
		    Internal_SetString(identifier, language, value);
		}

		public void RemoveString(string identifier)
		{
		    Internal_RemoveString(identifier);
		}

        public string GetLocalizedString(string identifier)
        {
            string value;
            Internal_GetLocalizedString(identifier, out value);
            return value;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetActiveLanguage(out Language value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetActiveLanguage(Language value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetString(string identifier, Language language, string value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_RemoveString(string identifier);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetLocalizedString(string identifier, out string value);
    }

    public enum Language
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
}
