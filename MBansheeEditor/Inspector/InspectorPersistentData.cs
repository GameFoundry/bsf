using BansheeEngine;
using System.Collections.Generic;

namespace BansheeEditor
{
    /// <summary>
    /// Contains Inspector specific data that should persist assembly refresh.
    /// </summary>
    internal class InspectorPersistentData : Component
    {
        [SerializeField]
        private Dictionary<ulong, Dictionary<string, object>> componentData =
            new Dictionary<ulong, Dictionary<string, object>>();

        [SerializeField] private Dictionary<string, Dictionary<string, object>> resourceData =
            new Dictionary<string, Dictionary<string, object>>();

        /// <summary>
        /// Returns existing, or creates new properties for a component with the specified id.
        /// </summary>
        /// <param name="componentId">Internal ID of the component to retrieve properties for.</param>
        /// <returns>A set of key value pairs representing persistent properties of an inspectable component.</returns>
        public Dictionary<string, object> GetProperties(ulong componentId)
        {
            Dictionary<string, object> output;
            if (!componentData.TryGetValue(componentId, out output))
            {
                output = new Dictionary<string, object>();
                componentData[componentId] = output;
            }

            return output;
        }

        /// <summary>
        /// Returns existing, or creates new properties for a resource with the specified UUID.
        /// </summary>
        /// <param name="uuid">Unique identifier of the resource to retrieve properties for.</param>
        /// <returns>A set of key value pairs representing persistent properties of an inspectable resource.</returns>
        public Dictionary<string, object> GetProperties(string uuid)
        {
            Dictionary<string, object> output;
            if (!resourceData.TryGetValue(uuid, out output))
            {
                output = new Dictionary<string, object>();
                resourceData[uuid] = output;
            }

            return output;
        }
    }
}
