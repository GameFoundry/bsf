using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public class ModalWindow : ScriptObject
    {
        public int Width
        {
            get { return Internal_GetWidth(mCachedPtr); }
            set { Internal_SetWidth(mCachedPtr, value); }
        }

        public int Height
        {
            get { return Internal_GetHeight(mCachedPtr); }
            set { Internal_SetHeight(mCachedPtr, value); }
        }

        protected GUIPanel GUI;

        protected ModalWindow()
        {
            Internal_CreateInstance(this, false);
        }

        protected ModalWindow(bool allowCloseButton)
        {
            Internal_CreateInstance(this, allowCloseButton);
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

        public LocString Title
        {
            set { Internal_SetTitle(mCachedPtr, value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(ModalWindow instance, bool allowCloseButton);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Close(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTitle(IntPtr nativeInstance, LocString title);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_InitializeGUIPanel(IntPtr nativeInstance, GUIPanel panel);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DestroyGUIPanel(IntPtr nativeInstance, GUIPanel panel);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetWidth(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_SetWidth(IntPtr nativeInstance, int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetHeight(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_SetHeight(IntPtr nativeInstance, int value);
    }
}
