using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class GUIPanelContainer : GUIElement
    {
        public GUIPanelContainer(GUIPanel panel, params GUIOption[] options)
        {
            Internal_CreateInstance(this, panel, options);
        }

        public GUIPanelContainer(GUIPanel panel)
        {
            Internal_CreateInstance(this, panel, new GUIOption[0]);
        }

        public void SetPanel(GUIPanel panel)
        {
            Internal_SetPanel(mCachedPtr, panel);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIPanelContainer instance, GUIPanel panel, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPanel(IntPtr nativeInstance, GUIPanel panel);
    }
}
