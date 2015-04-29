using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIPanelNEW : GUILayout
    {
        public GUIPanelNEW(ushort depth = 0, ushort depthRange = ushort.MaxValue, params GUIOption[] options)
        {
            Internal_CreateInstancePanel(this, depth, depthRange, options);
        }

        public GUIPanelNEW(params GUIOption[] options)
        {
            Internal_CreateInstancePanel(this, 0, ushort.MaxValue, options);
        }
    }
}
