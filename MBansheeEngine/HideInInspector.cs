using System;

namespace BansheeEngine
{
    /// <summary>
    /// Attribute that prevents a field from being visible in the inspector window in editor. Normally those fields
    /// are public fields of a <see cref="Component"/>, <see cref="Resource"/> or a class marked with a 
    /// <see cref="SerializeObject"/> attribute.
    /// </summary>
    [AttributeUsage(AttributeTargets.Field)]
    public sealed class HideInInspector : Attribute
    {
    }
}