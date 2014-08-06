using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class GUIIntField : GUIElement
    {
        public delegate void OnChangedDelegate(int newValue);

        public event OnChangedDelegate OnChanged;

        public int Value
        {
            get
            {
                int value; 
                Internal_GetValue(mCachedPtr, out value);
                return value;
            }

            set { Internal_SetValue(mCachedPtr, value); }
        }

        public GUIIntField(GUIContent title, int titleWidth = 100, string style = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, title, titleWidth, style, options, true);
        }

        public GUIIntField(string style = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, null, 0, style, options, false);
        }

        public bool HasInputFocus()
        {
            bool value;
            Internal_HasInputFocus(mCachedPtr, out value);
            return value;
        }

        public void SetRange(int min, int max)
        {
            Internal_SetRange(mCachedPtr, min, max);
        }

        private void DoOnChanged(int newValue)
        {
            if (OnChanged != null)
                OnChanged(newValue);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIIntField instance, GUIContent title, int titleWidth, 
            string style, GUIOption[] options, bool withTitle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetValue(IntPtr nativeInstance, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetValue(IntPtr nativeInstance, int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_HasInputFocus(IntPtr nativeInstance, out bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRange(IntPtr nativeInstance, int min, int max);
    }
}
