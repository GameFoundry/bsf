using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class GUITextureField : GUIElement
    {
        public delegate void OnChangedDelegate(Texture newValue);

        public event OnChangedDelegate OnChanged;

        public Texture Value
        {
            get
            {
                Texture value;
                Internal_GetValue(mCachedPtr, out value);
                return value;
            }

            set { Internal_SetValue(mCachedPtr, value); }
        }

        public GUITextureField(GUIContent title, int titleWidth = 100, string style = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, title, titleWidth, style, options, true);
        }

        public GUITextureField(string style = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, null, 0, style, options, false);
        }

        public void SetTint(Color color)
        {
            Internal_SetTint(mCachedPtr, color);
        }

        private void Internal_DoOnChanged(Texture newValue)
        {
            if (OnChanged != null)
                OnChanged(newValue);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUITextureField instance, GUIContent title, int titleWidth,
            string style, GUIOption[] options, bool withTitle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetValue(IntPtr nativeInstance, out Texture value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetValue(IntPtr nativeInstance, Texture value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTint(IntPtr nativeInstance, Color color);
    }
}
