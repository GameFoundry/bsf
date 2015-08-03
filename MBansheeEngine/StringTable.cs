using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public sealed class StringTable : Resource
    {
        public StringTable()
        {
            Internal_CreateInstance(this);
        }

        public int GetNumStrings()
        {
            return Internal_GetNumStrings(mCachedPtr);
        }

        public string[] GetIdentifiers()
        {
            return Internal_GetIdentifiers(mCachedPtr);
        }

        public void SetString(string identifier, Language language, string value)
        {
            Internal_SetString(mCachedPtr, identifier, language, value);
        }

        public void SetString(string identifier, string value)
        {
            Internal_SetStringDefault(mCachedPtr, identifier, value);
        }

        public void RemoveString(string identifier)
        {
            Internal_RemoveString(mCachedPtr, identifier);
        }

        public string GetString(string identifier, Language language)
        {
            string value;
            Internal_GetString(mCachedPtr, identifier, language, out value);
            return value;
        }

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
