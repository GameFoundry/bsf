using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class EditorGUI : GUIBase
    {
        internal EditorGUI(EditorWindow parentWindow)
        {
            Internal_CreateInstance(this, parentWindow);
            Initialize();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(EditorGUI instance, EditorWindow parentWindow);
    }
}
