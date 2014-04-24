using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BansheeEngine
{
    public sealed class SerializableValue
    {
        internal delegate void Setter(object value);
        internal delegate object Getter();

        private Type type;
        private Setter setter;
        private Getter getter;

        internal SerializableValue(Type type, Getter getter, Setter setter)
        {
            this.type = type;
            this.getter = getter;
            this.setter = setter;
        }

        public T GetValue<T>()
        {
            if (typeof (T) != type)
                throw new Exception("Attempted to retrieve a serializable value using an invalid type. Provided type: " + typeof(T) + ". Needed type: " + type);

            return (T) getter();
        }

        public void SetValue<T>(T value)
        {
            if (typeof(T) != type)
                throw new Exception("Attempted to set a serializable value using an invalid type. Provided type: " + typeof(T) + ". Needed type: " + type);

            setter(value);
        }
    }
}
