//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup Inspectors
     *  @{
     */

    /// <summary>
    /// Renders an inspector for the <see cref="PlaneCollider"/> component.
    /// </summary>
    [CustomInspector(typeof(PlaneCollider))]
    internal class PlaneColliderInspector : ColliderInspector
    {
        private GUIVector3Field normalField = new GUIVector3Field(new LocEdString("Normal"));
        private GUIFloatField distanceField = new GUIFloatField(new LocEdString("Distance"));

        private Vector3 normal;

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            PlaneCollider collider = InspectedObject as PlaneCollider;

            if (collider != null)
                BuildGUI(collider);
        }

        /// <inheritdoc/>
        protected internal override InspectableState Refresh()
        {
            PlaneCollider collider = InspectedObject as PlaneCollider;
            if (collider == null)
                return InspectableState.NotModified;

            Refresh(collider);

            InspectableState oldState = modifyState;
            if (modifyState.HasFlag(InspectableState.Modified))
                modifyState = InspectableState.NotModified;

            return oldState;
        }

        /// <summary>
        /// Creates GUI elements for fields specific to the plane collider.
        /// </summary>
        protected void BuildGUI(PlaneCollider collider)
        {
            normalField.OnChanged += x =>
            {
                normal = x;
                collider.Normal = x;

                MarkAsModified();
            };
            normalField.OnFocusLost += ConfirmModify;
            normalField.OnConfirmed += ConfirmModify;

            distanceField.OnChanged += x => { collider.Distance = x; MarkAsModified(); };
            distanceField.OnFocusLost += ConfirmModify;
            distanceField.OnConfirmed += ConfirmModify;

            Layout.AddElement(normalField);
            Layout.AddElement(distanceField);

            normal = collider.Normal;

            base.BuildGUI(collider);
        }

        /// <summary>
        /// Updates all GUI elements from current values in the collider.
        /// </summary>
        /// <param name="collider">Collider to update the GUI from.</param>
        protected void Refresh(PlaneCollider collider)
        {
            if (!normalField.HasInputFocus)
                normalField.Value = normal;

            if (!distanceField.HasInputFocus)
                distanceField.Value = collider.Distance;

            base.Refresh(collider);
        }
    }

    /** @} */
}