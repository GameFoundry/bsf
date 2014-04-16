using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public class EditorWindow : ScriptObject
    {
        internal int width { get { return Internal_GetWidth(); } }
        internal int height { get { return Internal_GetHeight(); } }

        protected EditorGUI GUI;

        public static EditorWindow OpenWindow<T>() where T : EditorWindow
        {
            return Internal_CreateOrGetInstance(typeof(T).Namespace, typeof(T).Name);
        }

        protected EditorWindow()
        {
            GUI = new EditorGUI(this);
        }

        protected virtual void WindowResized(int width, int height)
        {
            
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern EditorWindow Internal_CreateOrGetInstance(string ns, string typeName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetWidth();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetHeight();
    }
}
