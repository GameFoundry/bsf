using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public class EditorWindow : ScriptObject
    {
        internal int width { get { return Internal_GetWidth(mCachedPtr); } }
        internal int height { get { return Internal_GetHeight(mCachedPtr); } }

        protected GUIPanel GUI;

        public static EditorWindow OpenWindow<T>() where T : EditorWindow
        {
            return Internal_CreateOrGetInstance(typeof(T).Namespace, typeof(T).Name);
        }

        protected EditorWindow()
        {
            GUI = CreatePanel(0, 0, width, height);
        }

        protected virtual void WindowResized(int width, int height)
        {
            GUI.SetArea(0, 0, width, height);
        }

        internal GUIPanel CreatePanel(int x, int y, int width, int height)
        {
            GUIPanel newPanel = new GUIPanel();
            Internal_InitializeGUIPanel(mCachedPtr, newPanel);
            newPanel.Initialize();
            newPanel.SetArea(x, y, width, height);

            return newPanel;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern EditorWindow Internal_CreateOrGetInstance(string ns, string typeName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_InitializeGUIPanel(IntPtr nativeInstance, GUIPanel panel);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetWidth(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetHeight(IntPtr nativeInstance);
    }
}
