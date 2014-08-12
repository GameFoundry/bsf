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
    }

    public class Debug_Component1 : Component
    {
        public int value1;
        public int value2;
        public int[] intArray = new int[5];
        public DebugData data;
    }
}
