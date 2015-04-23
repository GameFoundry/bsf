using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUILabel : GUIElement
    {
        public GUILabel(GUIContent content, string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, content, style, options);
        }

        public GUILabel(GUIContent content, string style = "")
        {
            Internal_CreateInstance(this, content, style, new GUIOption[0]);
        }

        public GUILabel(GUIContent content, params GUIOption[] options)
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

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUILabel instance, GUIContent content, string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetContent(IntPtr nativeInstance, GUIContent content);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTint(IntPtr nativeInstance, Color color);
    }
}
