using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BansheeEngine
{
    public sealed class SerializableObject
    {
        private SerializableField[] _fields;

        public SerializableObject(object obj)
        {
            // TODO - Populate _fields
        }

        public SerializableField[] fields
        {
            get { return _fields; }
        }
    }
}
