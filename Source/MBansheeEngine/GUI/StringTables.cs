//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /** @addtogroup GUI-Engine
     *  @{
     */

    /// <summary>
    /// Manages string tables used for localizing text. Allows you to add and remove different tables and change the 
    /// active language.
    /// </summary>
    public static class StringTables
    {
        /// <summary>
        /// Currently active language that determines the translations retrieved from localized strings.
        /// </summary>
        public static Language ActiveLanguage
        {
            get
            {
                Language value;
                Internal_GetActiveLanguage(out value);
                return value;
            }
            set
            {
                Internal_SetActiveLanguage(value);
            }
        }

        /// <summary>
        /// Returns the string table with the specified id. If the table doesn't exist new one is created.
        /// </summary>
        /// <param name="id">Identifier of the string table.</param>
        /// <returns>String table with the specified identifier.</returns>
        public static StringTable GetTable(int id)
        {
            return Internal_GetTable(id);
        }

        /// <summary>
        /// Registers a new string table or replaces an old one at the specified id.
        /// </summary>
        /// <param name="id">Id of the string table to add/replace.</param>
        /// <param name="table">New string table to assign to the specified identifier.</param>
        public static void RegisterTable(int id, StringTable table)
        {
            Internal_SetTable(id, table);
        }

        /// <summary>
        /// Removes the string table with the specified id.
        /// </summary>
        /// <param name="id">Identifier of the table to remove.</param>
        public static void UnregisterTable(int id)
        {
            Internal_RemoveTable(id);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetActiveLanguage(out Language value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetActiveLanguage(Language value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern StringTable Internal_GetTable(int id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTable(int id, StringTable table);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_RemoveTable(int id);
    }

    /// <summary>
    /// A set of all languages that localized strings can be translated to. Loosely based on ISO 639-1 two letter language 
    /// codes.
    /// </summary>
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

    /** @} */
}
