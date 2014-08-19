using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class GUIFoldout : GUIElement
    {
        public delegate void OnToggledDelegate(bool expanded);

        public event OnToggledDelegate OnToggled;

        public GUIFoldout(GUIContent content, string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, content, style, options);
        }

        public GUIFoldout(GUIContent content, string style)
        {
            Internal_CreateInstance(this, content, style, new GUIOption[0]);
        }

        public GUIFoldout(GUIContent content, params GUIOption[] options)
        {
            Internal_CreateInstance(this, content, "", options);
        }

        public void SetContent(GUIContent content)
        {
            Internal_SetContent(mCachedPtr, content);
        }

        public bool IsExpanded()
        {
            bool expanded;
            Internal_IsExpanded(mCachedPtr, out expanded);
            return expanded;
        }

        public void SetExpanded(bool expanded)
        {
            Internal_SetExpanded(mCachedPtr, expanded);
        }

        private void DoOnToggled(bool expanded)
        {
            if (OnToggled != null)
                OnToggled(expanded);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIFoldout instance, GUIContent content, string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetContent(IntPtr nativeInstance, GUIContent content);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetExpanded(IntPtr nativeInstance, bool expanded);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_IsExpanded(IntPtr nativeInstance, out bool expanded);
    }
}
