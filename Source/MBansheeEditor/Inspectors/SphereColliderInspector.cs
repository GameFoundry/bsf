//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup Inspectors
     *  @{
     */

    /// <summary>
    /// Renders an inspector for the <see cref="SphereCollider"/> component.
    /// </summary>
    [CustomInspector(typeof(SphereCollider))]
    internal class SphereColliderInspector : ColliderInspector
    {
        private GUIVector3Field centerField = new GUIVector3Field(new LocEdString("Center"));
        private GUIFloatField radiusField = new GUIFloatField(new LocEdString("Radius"));

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            SphereCollider collider = InspectedObject as SphereCollider;

            if (collider != null)
                BuildGUI(collider);
        }

        /// <inheritdoc/>
        protected internal override InspectableState Refresh()
        {
            SphereCollider collider = InspectedObject as SphereCollider;
            if (collider == null)
                return InspectableState.NotModified;

            Refresh(collider);

            InspectableState oldState = modifyState;
            if (modifyState.HasFlag(InspectableState.Modified))
                modifyState = InspectableState.NotModified;

            return oldState;
        }

        /// <summary>
        /// Creates GUI elements for fields specific to the sphere collider.
        /// </summary>
        protected void BuildGUI(SphereCollider collider)
        {
            centerField.OnChanged += x => { collider.Center = x; MarkAsModified(); };
            centerField.OnFocusLost += ConfirmModify;
            centerField.OnConfirmed += ConfirmModify;

            radiusField.OnChanged += x => { collider.Radius = x; MarkAsModified(); };
            radiusField.OnFocusLost += ConfirmModify;
            radiusField.OnConfirmed += ConfirmModify;

            Layout.AddElement(centerField);
            Layout.AddElement(radiusField);

            base.BuildGUI(collider);
        }

        /// <summary>
        /// Updates all GUI elements from current values in the collider.
        /// </summary>
        /// <param name="collider">Collider to update the GUI from.</param>
        protected void Refresh(SphereCollider collider)
        {
            if (!centerField.HasInputFocus)
                centerField.Value = collider.Center;

            if (!radiusField.HasInputFocus)
                radiusField.Value = collider.Radius;

            base.Refresh(collider);
        }
    }

    /** @} */
}