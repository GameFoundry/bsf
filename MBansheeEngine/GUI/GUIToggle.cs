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
        public delegate void OnDoubleClickDelegate();

        public event OnClickDelegate OnClick;
        public event OnHoverDelegate OnHover;
        public event OnOutDelegate OnOut;
        public event OnToggleDelegate OnToggled;
        public event OnDoubleClickDelegate OnDoubleClick;

        public bool Value
        {
            get { return Internal_GetValue(mCachedPtr); }
            set { Internal_SetValue(mCachedPtr, value); }
        }

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

        public void SetTint(Color color)
        {
            Internal_SetTint(mCachedPtr, color);
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

        private void DoOnDoubleClick()
        {
            if (OnDoubleClick != null)
                OnDoubleClick();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIToggle instance, GUIContent content,
            GUIToggleGroup toggleGroup, string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetContent(IntPtr nativeInstance, GUIContent content);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_GetValue(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetValue(IntPtr nativeInstance, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTint(IntPtr nativeInstance, Color color);
    }
}
