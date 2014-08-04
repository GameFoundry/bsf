using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUILayoutX : GUILayout
    {
        internal GUILayoutX(GUIArea parentArea)
        {
            Internal_CreateInstanceXFromArea(this, parentArea);
        }

        internal GUILayoutX(GUILayout parentLayout)
        {
            Internal_CreateInstanceXFromLayoutAdd(this, parentLayout);
        }

        internal GUILayoutX(GUILayout parentLayout, int index)
        {
            Internal_CreateInstanceXFromLayoutInsert(this, parentLayout, index);
        }
    }
}
