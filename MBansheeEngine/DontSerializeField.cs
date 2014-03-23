using System;

namespace BansheeEngine
{
    [AttributeUsage(AttributeTargets.Field)]
    public sealed class DontSerializeField : Attribute
    {
    }
}
