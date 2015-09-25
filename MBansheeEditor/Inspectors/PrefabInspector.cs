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
        private bool isInitialized;

        /// <inheritdoc/>
        internal override bool Refresh()
        {
            Prefab prefab = referencedObject as Prefab;
            if (prefab == null)
                return false;

            if (!isInitialized)
            {
                // No GUI for prefab resource

                isInitialized = true;
            }

            return false;
        }
    }
}