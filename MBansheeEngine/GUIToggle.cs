using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIToggle : GUIElement
    {
        public delegate void OnClickDelegate();
        public delegate void OnHoverDelegate();
        public delegate void OnOutDelegate();
        public delegate void OnToggleDelegate(bool toggled);

        public event OnClickDelegate OnClick;
        public event OnHoverDelegate OnHover;
        public event OnOutDelegate OnOut;
        public event OnToggleDelegate OnToggled;

        internal GUIToggle(GUILayout parentLayout, GUIContent content, GUIToggleGroup toggleGroup, GUIElementStyle style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, parentLayout, content, toggleGroup, style, options);
        }

        public void SetContent(GUIContent content)
        {
            Internal_SetContent(mCachedPtr, content);
        }

        public void ToggleOn()
        {
            Internal_ToggleOn();
        }

        public void ToggleOff()
        {
            Internal_ToggleOff();
        }

        private void DoOnClick()
        {
            if (OnClick != null)
                OnClick();
        }

        private void DoOnHover()
        {
            if (OnHover != null)
                OnHover();
        }

        private void DoOnOut()
        {
            if (OnOut != null)
                OnOut();
        }

        private void DoOnToggled(bool toggled)
        {
            if (OnToggled != null)
                OnToggled(toggled);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIToggle instance, GUILayout layout, GUIContent content, 
            GUIToggleGroup toggleGroup, GUIElementStyle style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetContent(IntPtr nativeInstance, GUIContent content);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ToggleOn();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ToggleOff();
    }
}
