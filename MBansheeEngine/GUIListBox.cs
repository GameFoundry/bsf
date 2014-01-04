using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIListBox : ScriptObject
    {
        internal GUIListBox(GUILayout parentLayout, LocString[] elements, GUIElementStyle style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, parentLayout, elements, style, options);
        }

        public void SetElements(LocString[] elements)
        {
            Internal_SetElements(mCachedPtr, elements);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIListBox instance, GUILayout parentLayout, LocString[] elements, GUIElementStyle style, params GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetElements(IntPtr nativeInstance, LocString[] elements);
    }
}
