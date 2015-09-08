using System;

namespace BansheeEngine
{
    /// <summary>
    /// Attribute that may be used on fields to mark them as serializable. Normally public fields are serialized
    /// by default, and you can use this attribute to mark private/protected/internal fields as serialized as well.
    /// Fields must be in a serializable object otherwise this attribute has no effect.
    /// </summary>
    [AttributeUsage(AttributeTargets.Field)]
    public sealed class SerializeField : Attribute
    {
    }
}
