using System;

namespace BansheeEditor
{
    [AttributeUsage(AttributeTargets.Class)]
    public sealed class CustomInspector : Attribute
    {
        private Type type;

        public CustomInspector(Type type)
        {
            this.type = type;
        }
    }
}
