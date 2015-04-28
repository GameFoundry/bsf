using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUILayoutX : GUILayout
    {
        public GUILayoutX(params GUIOption[] options)
        {
            Internal_CreateInstanceX(this, options);
        }

        internal GUILayoutX(GUIArea parentArea)
        {
            Internal_CreateInstanceXFromArea(this, parentArea);
        }
    }
}
