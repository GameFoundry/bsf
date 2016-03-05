//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Handle to a localized string.
    /// </summary>
    public sealed class LocString : ScriptObject
    {
        /// <summary>
        /// Creates a new empty localized string.
        /// </summary>
        public LocString()
        {
            Internal_CreateInstance(this, "", 0);
        }

        /// <summary>
        /// Creates a new empty localized string.
        /// </summary>
        /// <param name="tableId">Unique identifier of the string table to retrieve the string from.</param>
        public LocString(int tableId)
        {
            Internal_CreateInstance(this, "", tableId);
        }

        /// <summary>
        /// Creates a new localized string with the specified identifier. If the identifier doesn't previously exist in 
        /// the string table, identifier value will also be used for initializing the default language version of the string.
        /// </summary>
        /// <param name="identifier">String you can use for later referencing the localized string.</param>
        /// <param name="tableId">Unique identifier of the string table to retrieve the string from.</param>
        public LocString(string identifier, int tableId = 0)
        {
            Internal_CreateInstance(this, identifier, tableId);
        }

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
            string value;
            Internal_GetValue(text.mCachedPtr, out value);
            return value;
        }

        /// <summary>
        /// Sets a value of a string parameter. Parameters are specified as bracketed values within the string itself 
        /// (e.g. {0}, {1}) etc. Use ^ as an escape character.
        /// </summary>
        /// <param name="idx">Index of the parameter to set.</param>
        /// <param name="value">Value to insert at the parameter's position.</param>
        public void SetParameter(int idx, string value)
        {
            Internal_SetParameter(mCachedPtr, idx, value);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(LocString instance, string identifier, int tableId);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetParameter(IntPtr nativeInstance, int idx, string identifier);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetValue(IntPtr nativeInstance, out string value);
    }
}
