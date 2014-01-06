using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUILayoutY : GUILayout
    {
        internal GUILayoutY(GUILayout parentLayout)
            :base(parentLayout)
        {
            Internal_CreateInstanceYFromLayout(this, parentLayout);
        }

        internal GUILayoutY(GUIScrollArea parentArea)
            :base(parentArea)
        {
            Internal_CreateInstanceYFromScrollArea(this, parentArea);
        }
    }
}
