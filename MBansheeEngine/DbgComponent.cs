using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BansheeEngine
{
    public class DbgComponent : Component
    {
        public int a;
        public DbgSerzObj complex = new DbgSerzObj();
        public DbgComponent2 otherComponent;
        public SceneObject otherSO;
        public int[,,] zeArray;
    }
}
