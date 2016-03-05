//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
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
        protected internal override InspectableState Refresh()
        {
            return InspectableState.NotModified;
        }
    }
}