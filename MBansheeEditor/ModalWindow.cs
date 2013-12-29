using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public class ModalWindow : ScriptObject
    {
        protected EditorGUI GUI;

        public ModalWindow(int top, int left, int width, int height)
        {
            Internal_CreateInstance(this, top, left, width, height);
            GUI = new EditorGUI(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(ModalWindow instance, int top, int left, int width, int height);
    }
}
