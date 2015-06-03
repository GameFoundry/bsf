using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUITextBox : GUIElement
    {
        public delegate void OnChangedDelegate(string newValue);

        public event OnChangedDelegate OnChanged;

        public GUITextBox(bool multiline, string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, multiline, style, options);
        }

        public GUITextBox(bool multiline, params GUIOption[] options)
        {
            Internal_CreateInstance(this, multiline, "", options);
        }

        public GUITextBox(string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, false, style, options);
        }

        public GUITextBox(params GUIOption[] options)
        {
            Internal_CreateInstance(this, false, "", options);
        }

        public string Text
        {
            get { string value; Internal_GetText(mCachedPtr, out value); return value; }
            set { Internal_SetText(mCachedPtr, value); }
        }

        public void SetTint(Color color)
        {
            Internal_SetTint(mCachedPtr, color);
        }

        private void DoOnChanged(string newValue)
        {
            if (OnChanged != null)
                OnChanged(newValue);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUITextBox instance, bool multiline, string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetText(IntPtr nativeInstance, string text);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetText(IntPtr nativeInstance, out string text);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTint(IntPtr nativeInstance, Color color);
    }
}
