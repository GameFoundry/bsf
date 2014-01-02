using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class HString : ScriptObject
    {
        public HString(string identifier)
        {
            Internal_CreateInstance(this, identifier);
        }

        public static implicit operator HString(string identifier)
        {
            return new HString(identifier);
        }

        public static implicit operator string(HString text)
        {
            string value;
            Internal_GetValue(text.mCachedPtr, out value);
            return value;
        }

        public void setParameter(int idx, string value)
        {
            Internal_SetParameter(mCachedPtr, idx, value);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(HString instance, string identifier);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetParameter(IntPtr nativeInstance, int idx, string identifier);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetValue(IntPtr nativeInstance, out string value);
    }
}
