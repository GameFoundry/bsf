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

        private Setter setter;
        private Getter getter;

        internal SerializableValue(Setter setter, Getter getter)
        {
            this.setter = setter;
            this.getter = getter;
        }

        public T GetValue<T>()
        {
            // TODO - Check for valid type?

            return (T) getter();
        }

        public void SetValue<T>(T value)
        {
            // TODO - Check for valid type?

            setter(value);
        }
    }
}
