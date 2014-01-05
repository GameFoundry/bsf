using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIListBox : GUIElement
    {
        public delegate void OnSelectionChangedDelegate(int index);

        public event OnSelectionChangedDelegate OnSelectionChanged;

        internal GUIListBox(GUILayout parentLayout, LocString[] elements, GUIElementStyle style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, parentLayout, elements, style, options);
        }

        public void SetElements(LocString[] elements)
        {
            Internal_SetElements(mCachedPtr, elements);
        }

        private void DoOnSelectionChanged(int index)
        {
            if (OnSelectionChanged != null)
                OnSelectionChanged(index);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIListBox instance, GUILayout parentLayout, LocString[] elements, GUIElementStyle style, params GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetElements(IntPtr nativeInstance, LocString[] elements);
    }
}
