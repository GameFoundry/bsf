using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public class ScriptCode : Resource
    {
        public string Text
        {
            get { return Internal_GetText(mCachedPtr); }
            set { Internal_SetText(mCachedPtr, value); }
        }

        public bool EditorScript
        {
            get { return Internal_IsEditorScript(mCachedPtr); }
            set { Internal_SetEditorScript(mCachedPtr, value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetText(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetText(IntPtr thisPtr, string value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsEditorScript(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetEditorScript(IntPtr thisPtr, bool value);
    }
}
