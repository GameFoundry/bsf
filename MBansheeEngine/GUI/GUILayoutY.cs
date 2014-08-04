using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUILayoutY : GUILayout
    {
        internal GUILayoutY(GUILayout parentLayout)
        {
            Internal_CreateInstanceYFromLayoutAdd(this, parentLayout);
        }

        internal GUILayoutY(GUIScrollArea parentArea)
        {
            Internal_CreateInstanceYFromScrollArea(this, parentArea);
        }

        internal GUILayoutY(GUILayout parentLayout, int index)
        {
            Internal_CreateInstanceYFromLayoutInsert(this, parentLayout, index);
        }
    }
}
