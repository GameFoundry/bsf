//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders an inspector for the <see cref="BoxCollider"/> component.
    /// </summary>
    [CustomInspector(typeof(BoxCollider))]
    public class BoxColliderInspector : ColliderInspector
    {
        private GUIVector3Field centerField = new GUIVector3Field(new LocEdString("Center"));
        private GUIVector3Field extentsField = new GUIVector3Field(new LocEdString("Extents"));

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            BoxCollider collider = InspectedObject as BoxCollider;

            if(collider != null)
                BuildGUI(collider);
        }

        /// <inheritdoc/>
        protected internal override InspectableState Refresh()
        {
            BoxCollider collider = InspectedObject as BoxCollider;
            if (collider == null)
                return InspectableState.NotModified;

            Refresh(collider);

            InspectableState oldState = modifyState;
            if (modifyState.HasFlag(InspectableState.Modified))
                modifyState = InspectableState.NotModified;

            return oldState;
        }

        /// <summary>
        /// Creates GUI elements for fields specific to the box collider.
        /// </summary>
        protected void BuildGUI(BoxCollider collider)
        {
            centerField.OnChanged += x => { collider.Center = x; MarkAsModified(); };
            centerField.OnFocusLost += ConfirmModify;
            centerField.OnConfirmed += ConfirmModify;

            extentsField.OnChanged += x => { collider.Extents = x; MarkAsModified(); };
            extentsField.OnFocusLost += ConfirmModify;
            extentsField.OnConfirmed += ConfirmModify;

            base.BuildGUI(collider);
        }

        /// <summary>
        /// Updates all GUI elements from current values in the collider.
        /// </summary>
        /// <param name="collider">Collider to update the GUI from.</param>
        protected void Refresh(BoxCollider collider)
        {
            centerField.Value = collider.Center;
            extentsField.Value = collider.Extents;

            base.Refresh(collider);
        }
    }
}