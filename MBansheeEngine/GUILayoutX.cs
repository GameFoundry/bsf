using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUILayoutX : GUILayout
    {
        internal GUILayoutX() { }

        internal GUILayoutX(GUIArea parentArea)
        {
            Internal_CreateInstanceXFromArea(this, parentArea);
        }

        internal GUILayoutX(GUILayout parentLayout)
        {
            Internal_CreateInstanceXFromLayout(this, parentLayout);
        }
    }
}
