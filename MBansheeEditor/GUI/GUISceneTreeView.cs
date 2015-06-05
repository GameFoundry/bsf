using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class GUISceneTreeView : GUIElement
    {
        public GUISceneTreeView(string style = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, style, options);
        }

        public GUISceneTreeView(params GUIOption[] options)
        {
            Internal_CreateInstance(this, "", options);
        }

        public void Update()
        {
            Internal_Update(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUISceneTreeView instance, string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Update(IntPtr thisPtr);
    }
}
