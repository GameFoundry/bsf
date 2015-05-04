using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIButton : GUIElement
    {
        public delegate void OnClickDelegate();
        public delegate void OnDoubleClickDelegate();
        public delegate void OnHoverDelegate();
        public delegate void OnOutDelegate();

        public event OnClickDelegate OnClick;
        public event OnDoubleClickDelegate OnDoubleClick;
        public event OnHoverDelegate OnHover;
        public event OnOutDelegate OnOut;

        public GUIButton(GUIContent content, string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, content, style, options);
        }

        public GUIButton(GUIContent content, string style)
        {
            Internal_CreateInstance(this, content, style, new GUIOption[0]);
        }

        public GUIButton(GUIContent content, params GUIOption[] options)
        {
            Internal_CreateInstance(this, content, "", options);
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

        private void DoOnDoubleClick()
        {
            if (OnDoubleClick != null)
                OnDoubleClick();
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

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIButton instance, GUIContent content, string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetContent(IntPtr nativeInstance, GUIContent content);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTint(IntPtr nativeInstance, Color color);
    }
}
