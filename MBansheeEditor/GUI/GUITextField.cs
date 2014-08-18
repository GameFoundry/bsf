using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class GUITextField : GUIElement
    {
        public delegate void OnChangedDelegate(String newValue);

        public event OnChangedDelegate OnChanged;

        public String Value
        {
            get
            {
                String value;
                Internal_GetValue(mCachedPtr, out value);
                return value;
            }

            set { Internal_SetValue(mCachedPtr, value); }
        }

        public GUITextField(GUIContent title, int titleWidth = 100, bool multiline = false, string style = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, multiline, title, titleWidth, style, options, true);
        }

        public GUITextField(bool multiline = false, string style = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, multiline, null, 0, style, options, false);
        }

        public bool HasInputFocus()
        {
            bool value;
            Internal_HasInputFocus(mCachedPtr, out value);
            return value;
        }

        private void DoOnChanged(String newValue)
        {
            if (OnChanged != null)
                OnChanged(newValue);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUITextField instance, bool multiline, GUIContent title, int titleWidth,
            string style, GUIOption[] options, bool withTitle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetValue(IntPtr nativeInstance, out String value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetValue(IntPtr nativeInstance, String value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_HasInputFocus(IntPtr nativeInstance, out bool value);
    }
}
