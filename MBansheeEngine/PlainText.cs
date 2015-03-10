using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public class PlainText : Resource
    {
        // For internal use by the runtime
        private PlainText()
        { }

        public PlainText(string text)
        {
            Internal_CreateInstance(this, text);
        }

        public string Text
        {
            get { return Internal_GetText(mCachedPtr); }
            set { Internal_SetText(mCachedPtr, value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(PlainText instance, string text);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetText(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetText(IntPtr thisPtr, string value);
    }
}
