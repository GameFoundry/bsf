using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUILayoutExplicit : GUILayout
    {
        internal GUILayoutExplicit(GUILayout parentLayout)
        {
            Internal_CreateInstanceExplicitFromLayoutAdd(this, parentLayout);
        }

        internal GUILayoutExplicit(GUILayout parentLayout, int index)
        {
            Internal_CreateInstanceExplicitFromLayoutInsert(this, parentLayout, index);
        }
    }
}
