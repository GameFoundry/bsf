//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders an inspector for the <see cref="Prefab"/> resource.
    /// </summary>
    [CustomInspector(typeof(Prefab))]
    internal class PrefabInspector : Inspector
    {
        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            // No GUI for prefab resource
        }

        /// <inheritdoc/>
        protected internal override InspectableState Refresh()
        {
            return InspectableState.NotModified;
        }
    }
}