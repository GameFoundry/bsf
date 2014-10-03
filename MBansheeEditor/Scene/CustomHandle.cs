using System;

namespace BansheeEditor
{
    [AttributeUsage(AttributeTargets.Class)]
    public sealed class CustomHandle : Attribute
    {
        private Type type;

        public CustomHandle(Type type)
        {
            this.type = type;
        }
    }
}
