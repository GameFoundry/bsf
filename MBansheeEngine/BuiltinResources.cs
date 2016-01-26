using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BansheeEngine
{
    class BuiltinResources
    {
        public Resource Load<T>(string path) where T : Resource, new()
        {
            // TODO -Dummy only
            return new T();
        }
    }
}
