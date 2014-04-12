using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIButton : GUIElement
    {
        public delegate void OnClickDelegate();
        public delegate void OnHoverDelegate();
        public delegate void OnOutDelegate();

        public event OnClickDelegate OnClick;
        public event OnHoverDelegate OnHover;
        public event OnOutDelegate OnOut;

        public GUIButton(GUIContent content, GUIElementStyle style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, content, style, options);
        }

        public GUIButton(GUIContent content, GUIElementStyle style)
        {
            Internal_CreateInstance(this, content, style, new GUIOption[0]);
        }

        public GUIButton(GUIContent content, params GUIOption[] options)
        {
            Internal_CreateInstance(this, content, null, options);
        }

        public void SetContent(GUIContent content)
        {
            Internal_SetContent(mCachedPtr, content);
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

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIButton instance, GUIContent content, GUIElementStyle style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetContent(IntPtr nativeInstance, GUIContent content);
    }
}
