using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUILayoutX : GUILayout
    {
        internal GUILayoutX(GUIArea parentArea)
            :base(null)
        {
            Internal_CreateInstanceXFromArea(this, parentArea);
        }

        internal GUILayoutX(GUILayout parentLayout)
            :base(parentLayout)
        {
            Internal_CreateInstanceXFromLayout(this, parentLayout);
        }
    }
}
