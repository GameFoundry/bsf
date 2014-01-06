using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUITextBox : GUIElement
    {
        internal GUITextBox(GUILayout parentLayout, bool multiline, GUIElementStyle style, params GUIOption[] options)
            :base(parentLayout)
        {
            Internal_CreateInstance(this, parentLayout, multiline, style, options);
        }

        public string text
        {
            get { string value; Internal_GetText(mCachedPtr, out value); return value; }
            set { Internal_SetText(mCachedPtr, value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUITextBox instance, GUILayout layout, bool multiline, GUIElementStyle style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetText(IntPtr nativeInstance, string text);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetText(IntPtr nativeInstance, out string text);
    }
}
