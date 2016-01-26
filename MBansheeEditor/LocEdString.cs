//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Helper object that creates a localized string in the editor-specific string table. Allows easier separation between
    /// editor and game string tables.
    /// </summary>
    internal class LocEdString
    {
        // Note: This must match C++ HEString::ENGINE_STRING_TABLE_ID
        private const int EDITOR_STRING_TABLE_ID = 30000;

        private LocString internalString;

        /// <summary>
        /// Creates a new empty localized string in the editor string table.
        /// </summary>
        public LocEdString()
        {
            internalString = new LocString(EDITOR_STRING_TABLE_ID);
        }

        /// <summary>
        /// Creates a new localized string with the specified identifier in the editor string table. If the identifier 
        /// doesn't previously exist in the string table, identifier value will also be used for initializing the default 
        /// language version of the string.
        /// </summary>
        /// <param name="identifier">String you can use for later referencing the localized string.</param>
        public LocEdString(string identifier)
        {
            internalString = new LocString(identifier, EDITOR_STRING_TABLE_ID);
        }

        /// <summary>
        /// Implicitly converts between an editor localized string and a normal localized string. The normal localized
        /// string will still internally reference the editor string table, making it equivalent to the original editor
        /// string aside from being different types.
        /// </summary>
        /// <param name="edString">Editor localized string to convert to normal localized string.</param>
        /// <returns>Normal localized string referencing the editor string table.</returns>
        public static implicit operator LocString(LocEdString edString)
        {
            return edString.internalString;
        }

        /// <summary>
        /// Implicity converts between an editor localized string and GUI contents. 
        /// </summary>
        /// <param name="edString">Editor localized string to convert.</param>
        /// <returns>GUI contents object that may be used for initializing various GUI elements.</returns>
        public static implicit operator GUIContent(LocEdString edString)
        {
            return new GUIContent(edString.internalString);
        }
    }
}
