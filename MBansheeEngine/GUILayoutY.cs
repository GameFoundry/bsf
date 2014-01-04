using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUILayoutY : GUILayout
    {
        internal GUILayoutY() { }

        internal GUILayoutY(GUILayout parentLayout)
        {
            Internal_CreateInstanceYFromLayout(this, parentLayout);
        }

        internal GUILayoutY(GUIScrollArea parentArea)
        {
            Internal_CreateInstanceYFromScrollArea(this, parentArea);
        }
    }
}
