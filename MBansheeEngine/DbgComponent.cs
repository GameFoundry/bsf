using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BansheeEngine
{
    public class DbgComponent : Component
    {
        public int a;
        public string b;
        public DbgSerzObj complex = new DbgSerzObj();
        public DbgSerzCls complex2 = new DbgSerzCls();

        public DbgComponent2 otherComponent;
        public SceneObject otherSO;

        //public int[][][] zeArray;
        //public List<DbgSerzObj> zeList;
        //public Dictionary<string, int> zeDict;
    }
}
