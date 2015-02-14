using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public class ModalWindow : ScriptObject
    {
        public int Width { get { return Internal_GetWidth(mCachedPtr); } }
        public int Height { get { return Internal_GetHeight(mCachedPtr); } }

        protected GUIPanel GUI;

        public static T Open<T>() where T : ModalWindow
        {
            return (T)Internal_CreateInstance(typeof(T).Namespace, typeof(T).Name);
        }

        private void OnInitializeInternal()
        {
            GUI = new GUIPanel();
            Internal_InitializeGUIPanel(mCachedPtr, GUI);
            GUI.Initialize();
            GUI.SetArea(0, 0, Width, Height);
        }

        private void OnDestroyInternal()
        {
            GUI.Destroy();
            Internal_DestroyGUIPanel(mCachedPtr, GUI);
        }

        protected virtual void OnWindowResized(int width, int height)
        {
            GUI.SetArea(0, 0, width, height);
        }

        protected void Close()
        {
            Internal_Close(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ModalWindow Internal_CreateInstance(string ns, string typeName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Close(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_InitializeGUIPanel(IntPtr nativeInstance, GUIPanel panel);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DestroyGUIPanel(IntPtr nativeInstance, GUIPanel panel);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetWidth(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetHeight(IntPtr nativeInstance);
    }
}
