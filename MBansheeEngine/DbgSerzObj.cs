using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BansheeEngine
{
    [SerializableObject]
    public struct DbgSerzObj
    {
        public DbgSerzObj(int someValue, string anotherValue)
        {
            this.someValue = someValue;
            this.anotherValue = anotherValue;
        }

        public int someValue;
        public string anotherValue;
    }
}
