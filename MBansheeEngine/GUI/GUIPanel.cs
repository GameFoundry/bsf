using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIPanel : GUILayout
    {
        private GUIPanel()
        { }

        public GUIPanel(Int16 depth = 0, ushort depthRangeMin = ushort.MaxValue, ushort depthRangeMax = ushort.MaxValue, params GUIOption[] options)
        {
            Internal_CreateInstancePanel(this, depth, depthRangeMin, depthRangeMax, options);
        }

        public GUIPanel(params GUIOption[] options)
        {
            Internal_CreateInstancePanel(this, 0, ushort.MaxValue, ushort.MaxValue, options);
        }
    }
}
