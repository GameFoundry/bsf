using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    /// <summary>
    /// Used for string localization. Stores strings and their translations in various languages.
    /// </summary>
    public sealed class StringTable : Resource
    {
        /// <summary>
        /// Constructor for runtime use only.
        /// </summary>
        /// <param name="dummy">Dummy parameter to differentiate from the normal constructor.</param>
        private StringTable(bool dummy)
        {  }

        /// <summary>
        /// Creates a new empty string table.
        /// </summary>
        public StringTable()
        {
            Internal_CreateInstance(this);
        }

        /// <summary>
        /// Returns the total number of strings in the table.
        /// </summary>
        public int StringCount
        {
            get { return Internal_GetNumStrings(mCachedPtr); }
        }

        /// <summary>
        /// Returns all identifiers that the string table contains localized strings for.
        /// </summary>
        public string[] Identifiers
        {
            get { return Internal_GetIdentifiers(mCachedPtr); }
        }

        /// <summary>
        /// Adds or modifies string translation for the specified language.
        /// </summary>
        /// <param name="identifier">Identifier of the string to add/modify.</param>
        /// <param name="language">Language to add/modify the translation for.</param>
        /// <param name="value">Translated string in the specified language.</param>
        public void SetString(string identifier, Language language, string value)
        {
            Internal_SetString(mCachedPtr, identifier, language, value);
        }

        /// <summary>
        /// Adds or modifies string translation for the currently active language.
        /// </summary>
        /// <param name="identifier">Identifier of the string to add/modify.</param>
        /// <param name="value">Translated string in the active language.</param>
        public void SetString(string identifier, string value)
        {
            Internal_SetStringDefault(mCachedPtr, identifier, value);
        }

        /// <summary>
        /// Removes the string described by identifier, from all languages.
        /// </summary>
        /// <param name="identifier">Identifier of the string to remove.</param>
        public void RemoveString(string identifier)
        {
            Internal_RemoveString(mCachedPtr, identifier);
        }

        /// <summary>
        /// Returns a string translation for the specified language.
        /// </summary>
        /// <param name="identifier">Identifier of the string to look up.</param>
        /// <param name="language">Language to retrieve the translation for.</param>
        /// <returns>String translation for the specified language. Returns the identifier itself if one doesn't exist.
        /// </returns>
        public string GetString(string identifier, Language language)
        {
            string value;
            Internal_GetString(mCachedPtr, identifier, language, out value);
            return value;
        }

        /// <summary>
        /// Returns a string translation for the currently active language.
        /// </summary>
        /// <param name="identifier">Identifier of the string to look up.</param>
        /// <returns>String translation for the active language. Returns the identifier itself if one doesn't exist.
        /// </returns>
        public string GetString(string identifier)
        {
            string value;
            Internal_GetStringDefault(mCachedPtr, identifier, out value);
            return value;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(StringTable instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetNumStrings(IntPtr thisPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string[] Internal_GetIdentifiers(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetString(IntPtr thisPtr, string identifier, Language language, string value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetStringDefault(IntPtr thisPtr, string identifier, string value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_RemoveString(IntPtr thisPtr, string identifier);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetString(IntPtr thisPtr, string identifier, Language language, out string value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetStringDefault(IntPtr thisPtr, string identifier, out string value);
    }
}
