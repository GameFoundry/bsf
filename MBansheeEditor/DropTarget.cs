using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public class DropTarget : ScriptObject
    {
        public Action<int, int> OnDrop;
        public Action<int, int> OnEnter;
        public Action OnLeave;
        public Action<int, int> OnDrag;

        public DropTarget(EditorWindow window)
        {
            IntPtr nativeWindow = window.GetCachedPtr();;
            Internal_CreateInstance(this, nativeWindow);
        }

        public void Destroy()
        {
            Internal_Destroy(mCachedPtr);
        }

        public Rect2I Bounds
        {
            set { Internal_SetBounds(mCachedPtr, value); }
        }

        public string[] FilePaths
        {
            get { return Internal_GetFilePaths(mCachedPtr); }
        }

        private void InternalDoOnEnter(int x, int y)
        {
            if (OnEnter != null)
                OnEnter(x, y);
        }

        private void InternalDoOnLeave()
        {
            if (OnLeave != null)
                OnLeave();
        }

        private void InternalDoOnDrag(int x, int y)
        {
            if (OnDrag != null)
                OnDrag(x, y);
        }

        private void InternalDoOnDrop(int x, int y)
        {
            if (OnDrop != null)
                OnDrop(x, y);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(DropTarget instance, IntPtr editorWindow);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Destroy(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetBounds(IntPtr nativeInstance, Rect2I bounds);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string[] Internal_GetFilePaths(IntPtr nativeInstance);
    }
}
