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
            get { return Internal_GetValue(mCachedPtr); }
            set { Internal_SetValue(mCachedPtr, value); }
        }

        public GUIIntField(GUIContent title, int titleWidth = 100, string titleStyle = "",
                           string inputStyle = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, title, titleWidth, titleStyle, inputStyle, options, true);
        }

        public GUIIntField(string inputStyle = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, null, 0, "", inputStyle, options, false);
        }

        private void DoOnChanged(int newValue)
        {
            if (OnChanged != null)
                OnChanged(newValue);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIIntField instance, GUIContent title, int titleWidth, 
            string titleStyle, string inputStyle, GUIOption[] options, bool withTitle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetValue(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetValue(IntPtr nativeInstance, int value);
    }
}
