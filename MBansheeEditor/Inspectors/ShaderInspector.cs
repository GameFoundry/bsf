using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders an inspector for the <see cref="Shader"/> resource.
    /// </summary>
    [CustomInspector(typeof(Shader))]
    internal class ShaderInspector : Inspector
    {
        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            // No GUI for shader resource
        }

        /// <inheritdoc/>
        protected internal override void Refresh()
        {
        }
    }
}