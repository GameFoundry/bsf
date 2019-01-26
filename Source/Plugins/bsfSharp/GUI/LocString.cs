//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /** @addtogroup GUI
     *  @{
     */

    public partial class LocString
    {
        /// <summary>
        /// Converts a normal string into a localized string by using the normal string as the identifier.
        /// </summary>
        /// <param name="identifier">Localized string identifier.</param>
        /// <returns>Localized string with the provided identifier, from the default string table.</returns>
        public static implicit operator LocString(string identifier)
        {
            return new LocString(identifier);
        }

        /// <summary>
        /// Retrieves localized text for the active language from the localized string.
        /// </summary>
        /// <param name="text">Localized string to retrieve the text from.</param>
        /// <returns>Translated text for the currently active language.</returns>
        public static explicit operator string(LocString text)
        {
            return Internal_getValue(text.mCachedPtr);
        }
    }

    /** @} */
}
