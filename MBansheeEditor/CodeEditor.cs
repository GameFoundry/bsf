using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    // Note: Must match C++ CodeEditorType enum
    public enum CodeEditorType
    {
        VS2008,
        VS2010,
        VS2012,
        VS2013,
        VS2015
    }

    public static class CodeEditor
    {
        public static CodeEditorType ActiveEditor
        {
            set { Internal_SetActiveEditor(value); }
        }

        public static CodeEditorType[] AvailableEditors
        {
            get { return Internal_GetAvailableEditors(); }
        }

        public static void OpenFile(string path, UInt32 line)
        {
            Internal_OpenFile(path, line);
        }

        public static void SyncSolution()
        {
            Internal_SyncSolution();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_SetActiveEditor(CodeEditorType type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern CodeEditorType[] Internal_GetAvailableEditors();

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_OpenFile(string path, UInt32 line);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_SyncSolution();
    }
}
