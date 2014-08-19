using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BansheeEngine
{
    [SerializeObject]
    public class DebugData
    {
        public int val3;
        public int val4;
        public float fltVal;
        public bool boolVal;
        public string strVal;
        public Color colVal;
        public Vector2 vec2Val;
        public Vector3 vec3Val;
        public Vector4 vec4Val;
        public Component cmp1;
        public Texture2D tex1;
    }

    public class Debug_Component1 : Component
    {
        public int value1;
        public int value2;
        public int[] intArray = new int[1];
        public DebugData data;
    }
}
