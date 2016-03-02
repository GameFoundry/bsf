//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders an inspector for the <see cref="MeshCollider"/> component.
    /// </summary>
    [CustomInspector(typeof(MeshCollider))]
    public class MeshColliderInspector : ColliderInspector
    {
        private GUIResourceField meshField = new GUIResourceField(typeof(PhysicsMesh), new LocEdString("Mesh"));

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            MeshCollider collider = InspectedObject as MeshCollider;

            if (collider != null)
                BuildGUI(collider);
        }

        /// <inheritdoc/>
        protected internal override InspectableState Refresh()
        {
            MeshCollider collider = InspectedObject as MeshCollider;
            if (collider == null)
                return InspectableState.NotModified;

            Refresh(collider);

            InspectableState oldState = modifyState;
            if (modifyState.HasFlag(InspectableState.Modified))
                modifyState = InspectableState.NotModified;

            return oldState;
        }

        /// <summary>
        /// Creates GUI elements for fields specific to the mesh collider.
        /// </summary>
        protected void BuildGUI(MeshCollider collider)
        {
            meshField.OnChanged += x =>
            {
                PhysicsMesh mesh = Resources.Load<PhysicsMesh>(x);
                collider.Mesh = mesh;

                MarkAsModified();
                ConfirmModify();
            };

            base.BuildGUI(collider);
        }

        /// <summary>
        /// Updates all GUI elements from current values in the collider.
        /// </summary>
        /// <param name="collider">Collider to update the GUI from.</param>
        protected void Refresh(MeshCollider collider)
        {
            meshField.Value = collider.Mesh;

            base.Refresh(collider);
        }
    }
}