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

        public int[] arrA;
        public string[] arrB;
        public DbgSerzObj[] arrComplex;
        public DbgSerzCls[] arrComplex2;

        public List<int> listA;
        public List<string> listB;
        public List<DbgSerzObj> listComplex;
        public List<DbgSerzCls> listComplex2;

        public DbgComponent2 otherComponent;
        public SceneObject otherSO;

        //public int[][][] zeArray;
        //public List<DbgSerzObj> zeList;
        //public Dictionary<string, int> zeDict;
    }
}
