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

        public GUIToggle(GUIContent content, GUIToggleGroup toggleGroup, string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, content, toggleGroup, style, options);
        }

        public GUIToggle(GUIContent content, string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, content, null, style, options);
        }

        public GUIToggle(GUIContent content, string style)
        {
            Internal_CreateInstance(this, content, null, style, new GUIOption[0]);
        }

        public GUIToggle(GUIContent content, params GUIOption[] options)
        {
            Internal_CreateInstance(this, content, null, "", options);
        }

        public GUIToggle(GUIContent content, GUIToggleGroup toggleGroup, string style)
        {
            Internal_CreateInstance(this, content, toggleGroup, style, new GUIOption[0]);
        }

        public GUIToggle(GUIContent content, GUIToggleGroup toggleGroup, params GUIOption[] options)
        {
            Internal_CreateInstance(this, content, toggleGroup, "", options);
        }

        public void SetContent(GUIContent content)
        {
            Internal_SetContent(mCachedPtr, content);
        }

        public void ToggleOn()
        {
            Internal_ToggleOn(mCachedPtr);
        }

        public void ToggleOff()
        {
            Internal_ToggleOff(mCachedPtr);
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
        private static extern void Internal_CreateInstance(GUIToggle instance, GUIContent content,
            GUIToggleGroup toggleGroup, string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetContent(IntPtr nativeInstance, GUIContent content);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ToggleOn(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ToggleOff(IntPtr nativeInstance);
    }
}
