//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders an inspector for the <see cref="CapsuleCollider"/> component.
    /// </summary>
    [CustomInspector(typeof(CapsuleCollider))]
    public class CapsuleColliderInspector : ColliderInspector
    {
        private GUIVector3Field centerField = new GUIVector3Field(new LocEdString("Center"));
        private GUIVector3Field orientationField = new GUIVector3Field(new LocEdString("Orientation"));
        private GUIFloatField radiusField = new GUIFloatField(new LocEdString("Radius"));
        private GUIFloatField halfHeightField = new GUIFloatField(new LocEdString("Half height"));

        private Vector3 orientation;

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            CapsuleCollider collider = InspectedObject as CapsuleCollider;

            if (collider != null)
                BuildGUI(collider);
        }

        /// <inheritdoc/>
        protected internal override InspectableState Refresh()
        {
            CapsuleCollider collider = InspectedObject as CapsuleCollider;
            if (collider == null)
                return InspectableState.NotModified;

            Refresh(collider);

            InspectableState oldState = modifyState;
            if (modifyState.HasFlag(InspectableState.Modified))
                modifyState = InspectableState.NotModified;

            return oldState;
        }

        /// <summary>
        /// Creates GUI elements for fields specific to the capsule collider.
        /// </summary>
        protected void BuildGUI(CapsuleCollider collider)
        {
            centerField.OnChanged += x => { collider.Center = x; MarkAsModified(); };
            centerField.OnFocusLost += ConfirmModify;
            centerField.OnConfirmed += ConfirmModify;

            orientationField.OnChanged += x =>
            {
                orientation = x;
                Quaternion rotation = Quaternion.FromEuler(x);

                collider.Normal = rotation.Rotate(Vector3.YAxis);
                MarkAsModified();
            };
            orientationField.OnFocusLost += ConfirmModify;
            orientationField.OnConfirmed += ConfirmModify;

            radiusField.OnChanged += x => { collider.Radius = x; MarkAsModified(); };
            radiusField.OnFocusLost += ConfirmModify;
            radiusField.OnConfirmed += ConfirmModify;

            halfHeightField.OnChanged += x => { collider.HalfHeight = x; MarkAsModified(); };
            halfHeightField.OnFocusLost += ConfirmModify;
            halfHeightField.OnConfirmed += ConfirmModify;

            Layout.AddElement(centerField);
            Layout.AddElement(orientationField);
            Layout.AddElement(radiusField);
            Layout.AddElement(halfHeightField);

            orientation = Quaternion.FromToRotation(Vector3.YAxis, collider.Normal).ToEuler();

            base.BuildGUI(collider);
        }

        /// <summary>
        /// Updates all GUI elements from current values in the collider.
        /// </summary>
        /// <param name="collider">Collider to update the GUI from.</param>
        protected void Refresh(CapsuleCollider collider)
        {
            if (!centerField.HasInputFocus)
                centerField.Value = collider.Center;

            if (!orientationField.HasInputFocus)
                orientationField.Value = orientation;

            if (!radiusField.HasInputFocus)
                radiusField.Value = collider.Radius;

            if (!halfHeightField.HasInputFocus)
                halfHeightField.Value = collider.HalfHeight;

            base.Refresh(collider);
        }
    }
}